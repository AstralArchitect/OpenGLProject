#include <tools/gltfloader.hpp>

#include <cstdio>
#include <iostream>
#include <algorithm>

#include <glad/glad.h>
#include <functions.hpp>

#define btogl(x) x ? GL_TRUE : GL_FALSE
#define tget(x, y) std::get<x>(y)

GltfModel GltfModel::loadWithPath(const char* filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    tinygltf::Model model;
    std::vector<GltfNode> children;

    bool res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        printf("[Gltf] WARN: %s\n", warn);
    }

    if (!err.empty()) {
        printf("[Gltf] ERR: %s\n", err);
    }

    if (!res) printf("Failed to load glTF: %s\n", filename);
    else printf("Loaded glTF: %s\n", filename);

    const tinygltf::Scene &scene = model.scenes[model.defaultScene];
    std::vector<GltfNode> loaded_nodes;
    for (size_t i = 0; i < scene.nodes.size(); ++i) {
        assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
        loaded_nodes.push_back(GltfNode(model, model.nodes[scene.nodes[i]]));
    }

    children = loaded_nodes;

    GltfModel out;
    out.children = children;
    out.model = model;

    return out;
}

void GltfModel::draw() {
    for (GltfNode &child: this->children) {
        child.draw();
    }
}

GltfNode::GltfNode(tinygltf::Model &root, tinygltf::Node node) {
    mesh = ((node.mesh >= 0) && (node.mesh < root.meshes.size())) ?
        std::optional(GltfMesh(root, root.meshes[node.mesh])) :
        std::nullopt;


    for (size_t i = 0; i < node.children.size(); i++) {
        assert((node.children[i] >= 0) && (node.children[i] < root.nodes.size()));
        children.push_back(GltfNode(root, root.nodes[node.children[i]]));
    }
}

void GltfNode::draw() {
    for (GltfNode &child: this->children) {
        child.draw();
    }

    if (this->mesh.has_value()) {
        this->mesh.value().draw();
    }
}

GltfMesh::GltfMesh(tinygltf::Model &root, tinygltf::Mesh mesh) {
    for (const tinygltf::Primitive &prim : mesh.primitives) {
        primitives.push_back(GltfPrimitive::GltfPrimitive(root, prim));
    }
}

// Tuple printer (https://stackoverflow.com/a/41171552)
template <class TupleType, size_t... I>
void print_tuple_util(const TupleType &tuple_to_print, std::index_sequence<I...>) {
    std::cout << "(";
    (..., (std::cout << (I == 0? "" : ", ") << std::get<I>(tuple_to_print)));
    std::cout << ")" << std::endl;
}

template <class... T>
void print_tuple(const std::tuple<T...> &tuple_to_print) {
    print_tuple_util(tuple_to_print, std::make_index_sequence<sizeof...(T)>());
}

void GltfMesh::draw() {
    for (const auto &prim: this->primitives) {
        prim.draw();
    }
}

GLuint load_texture_to_gpu(tinygltf::Model &root, tinygltf::TextureInfo texinfo) {
    assert((texinfo.index >= 0) && (texinfo.index < root.textures.size()));
    tinygltf::Texture gltftex = root.textures[texinfo.index];

    assert((gltftex.source >= 0) && (gltftex.source < root.images.size()));
    tinygltf::Image image = root.images[gltftex.source];

    std::for_each_n(image.image.cbegin(), 10, [](const unsigned char &el) {printf("%hhu, ", el);});

    assert((gltftex.sampler >= 0) && (gltftex.sampler < root.textures.size()));
    tinygltf::Sampler sampler = root.samplers[gltftex.sampler];

    GLenum tex_components, tex_bits;
    switch (image.component) {
        case 3:
            tex_components = GL_RGB;
            tex_bits = GL_RGB8;
            break;

        case 4:
            tex_components = GL_RGBA;
            tex_bits = GL_RGBA8;
            break;
        default:
            printf("%d components isn't supported right now");
            throw -1;
    }

    assert(image.as_is == false);

    printf("Uploading to GPU\n");
    GLuint gputex;
    glGenTextures(1, &gputex);
    glBindTexture(GL_TEXTURE_2D, gputex);
    glTexImage2D(GL_TEXTURE_2D, 0, tex_components, image.width, image.height, 0, tex_components, image.pixel_type, &image.image);
    glGenerateMipmap(GL_TEXTURE_2D);
    printf("Uploading parameters...");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
    printf("Upload finished !");

    return gputex;
}

GltfMaterial::GltfMaterial(tinygltf::Model &root, tinygltf::Material material) {
    tinygltf::TextureInfo basecolor_texinfo = material.pbrMetallicRoughness.baseColorTexture;
    tinygltf::TextureInfo metallic_roughness_texinfo = material.pbrMetallicRoughness.metallicRoughnessTexture;

    printf("Loading textures...\n");
    basecolor_gputex = load_texture_to_gpu(root, basecolor_texinfo);
    metallic_roughness_gputex = load_texture_to_gpu(root, metallic_roughness_texinfo);

    printf("Textures loaded !\n");
}

void GltfMaterial::activate() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, basecolor_gputex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, metallic_roughness_gputex);
}

GltfPrimitive::GltfPrimitive(tinygltf::Model &root, const tinygltf::Primitive &prim) {
    glGenVertexArrays(1, &vao);
    printf("VAO num: %lu\n", vao);
    glBindVertexArray(vao);

    typedef struct {
        tinygltf::Accessor accessor;
        tinygltf::BufferView buffer_view;
        tinygltf::Buffer buffer;
    } BufferInfos;
    std::optional<BufferInfos> pos_buf_opt;
    std::optional<BufferInfos> normal_buf;
    std::optional<BufferInfos> texcoord_buf;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    int stride = 3;

    for (const auto &attr : prim.attributes) {
        assert((attr.second >= 0) && (attr.second < root.accessors.size()));
        tinygltf::Accessor accessor = root.accessors[attr.second];
        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

        assert((accessor.bufferView >= 0) && (accessor.bufferView < root.bufferViews.size()));
        tinygltf::BufferView buffer_view = root.bufferViews[accessor.bufferView];

        assert((buffer_view.buffer >= 0) && (buffer_view.buffer < root.buffers.size()));
        tinygltf::Buffer buffer = root.buffers[buffer_view.buffer];

        if (attr.first.compare("POSITION") == 0) {
            pos_buf_opt = std::optional(BufferInfos { accessor, buffer_view, buffer });
            printf("model has positions\n");
        } else if (attr.first.compare("NORMAL") == 0) {
            normal_buf = std::optional(BufferInfos { accessor, buffer_view, buffer });
            stride += 3;
            printf("model has normals\n");
        } else if (attr.first.compare("TEXCOORD_0") == 0) {
            texcoord_buf = std::optional(BufferInfos { accessor, buffer_view, buffer });
            stride += 2;
            printf("model has tex coords\n");
        } else {
            printf("[Gltf] Unsupported attributes found in model: %d -> Skipping !\n",  attr.first);
            continue;
        }
    }
    assert(pos_buf_opt.has_value());

    BufferInfos pos_buf = pos_buf_opt.value();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    if (texcoord_buf) {
        glVertexAttribPointer(1, 2, GL_FLOAT, btogl(texcoord_buf.value().accessor.normalized), stride * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    if (normal_buf) {
        glVertexAttribPointer(2, 3, GL_FLOAT, btogl(normal_buf.value().accessor.normalized), stride * sizeof(float), (void*)((stride - 3) * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    printf("Vertex count: %zu\n", pos_buf.accessor.count);

    std::vector<unsigned char> final_buffer;
    for (int idx = 0; idx < pos_buf.accessor.count; idx++) {
        final_buffer.insert(
            final_buffer.cend(),
            pos_buf.buffer.data.cbegin() + pos_buf.buffer_view.byteOffset + (idx * 3 * sizeof(float)),
            pos_buf.buffer.data.cbegin() + pos_buf.buffer_view.byteOffset + ((idx + 1) * 3 * sizeof(float))
        );

        if (texcoord_buf) {
            final_buffer.insert(
                final_buffer.cend(),
                texcoord_buf.value().buffer.data.cbegin() + texcoord_buf.value().buffer_view.byteOffset + (idx * 2 * sizeof(float)),
                texcoord_buf.value().buffer.data.cbegin() + texcoord_buf.value().buffer_view.byteOffset + ((idx + 1) * 2 * sizeof(float))
            );
        }

        if (normal_buf) {
            final_buffer.insert(
                final_buffer.cend(),
                normal_buf.value().buffer.data.cbegin() + normal_buf.value().buffer_view.byteOffset + (idx * 3 * sizeof(float)),
                normal_buf.value().buffer.data.cbegin() + normal_buf.value().buffer_view.byteOffset + ((idx + 1) * 3 * sizeof(float))
            );
        }
    }

    glBufferData(GL_ARRAY_BUFFER, final_buffer.size(), final_buffer.data(), GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    assert((prim.indices >= 0) && (prim.indices < root.accessors.size()));
    tinygltf::Accessor indices_accessor = root.accessors[prim.indices];
    
    assert((indices_accessor.bufferView >= 0) && (indices_accessor.bufferView < root.bufferViews.size()));
    tinygltf::BufferView indices_buffer_view = root.bufferViews[indices_accessor.bufferView];

    assert((indices_buffer_view.buffer >= 0) && (indices_buffer_view.buffer < root.buffers.size()));
    tinygltf::Buffer indices_buffer = root.buffers[indices_buffer_view.buffer];

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_view.byteLength, indices_buffer.data.data() + indices_buffer_view.byteOffset, GL_STATIC_DRAW);

    glBindVertexArray(0);

    assert((prim.material >= 0) && (prim.material < root.materials.size()));
    tinygltf::Material mat = root.materials[prim.material];

    material = GltfMaterial::GltfMaterial(root, mat);

    draw_mode = prim.mode;
    vertex_count = indices_accessor.count;
}

void GltfPrimitive::draw() const {
    glBindVertexArray(vao);

    material.activate();

    glDrawElements(draw_mode, vertex_count, GL_UNSIGNED_SHORT, (void*)0);
    glBindVertexArray(0);
}
