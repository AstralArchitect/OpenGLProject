#include <tools/gltfloader.hpp>

#include <iostream>

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
        std::cout << "[Gltf] WARN: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "[Gltf] ERR: " << err << std::endl;
    }

    if (!res) std::cout << "Failed to load glTF: " << filename << std::endl;
    else std::cout << "Loaded glTF: " << filename << std::endl;

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
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        std::cout << "VAO num: " << vao << std::endl;
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
                std::cout << "model has positions" << std::endl;
            } else if (attr.first.compare("NORMAL") == 0) {
                normal_buf = std::optional(BufferInfos { accessor, buffer_view, buffer });
                stride += 3;
                std::cout << "model has normals" << std::endl;
            } else if (attr.first.compare("TEXCOORD_0") == 0) {
                texcoord_buf = std::optional(BufferInfos { accessor, buffer_view, buffer });
                stride += 2;
                std::cout << "model has tex coords" << std::endl;
            } else {
                std::cout << "[Gltf] Unsupported attributes found in model: " << attr.first << " -> Skipping !" << std::endl;
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

        std::cout << "Vertex count: " << pos_buf.accessor.count << std::endl;

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

        if (final_buffer.size() % sizeof(float) == 0) {
            std::cout << "Final Buf: [";
            
            float * ptr = (float*)final_buffer.data();
            for (int cnt = 0; cnt < final_buffer.size() / sizeof(float); cnt++) {
                std::cout << *ptr << ", ";
                ptr++;
            }

            std::cout << std::endl;
        } else {
            std::cout << "Can't print buffer: anormal size" << std::endl;
        }

        glBufferData(GL_ARRAY_BUFFER, final_buffer.size(), final_buffer.data(), GL_STATIC_DRAW);

        float * fptr = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, final_buffer.size(), GL_MAP_READ_BIT);

        unsigned short counter = 0;

        std::cout << "Final Buf (GPU MEM):";
        for (int cnt = 0; cnt < final_buffer.size() / sizeof(float); cnt++) {
            if (cnt % stride == 0) {
                std::cout << std::endl << counter << ": ";
                counter++;
            }
            std::cout << *fptr << ", ";
            fptr++;
        }

        std::cout << std::endl;

        glUnmapBuffer(GL_ARRAY_BUFFER);

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        assert((prim.indices >= 0) && (prim.indices < root.accessors.size()));
        tinygltf::Accessor indices_accessor = root.accessors[prim.indices];
        
        assert((indices_accessor.bufferView >= 0) && (indices_accessor.bufferView < root.bufferViews.size()));
        tinygltf::BufferView indices_buffer_view = root.bufferViews[indices_accessor.bufferView];

        assert((indices_buffer_view.buffer >= 0) && (indices_buffer_view.buffer < root.buffers.size()));
        tinygltf::Buffer indices_buffer = root.buffers[indices_buffer_view.buffer];

        std::cout << "Indices buffer (" << indices_buffer_view.byteLength / sizeof(unsigned short) << "): ";

        unsigned char * ptr = indices_buffer.data.data();
        ptr += indices_buffer_view.byteOffset;
        unsigned short * sptr = (unsigned short *)ptr;
        for (unsigned short cnt = 0; cnt < indices_buffer_view.byteLength / sizeof(unsigned short); cnt++) {
            std::cout << *sptr << ", ";
            sptr++;
        }
        std::cout << std::endl;

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_view.byteLength, indices_buffer.data.data() + indices_buffer_view.byteOffset, GL_STATIC_DRAW);
        
        unsigned short * iptr = (unsigned short*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indices_buffer_view.byteLength, GL_MAP_READ_BIT);

        std::cout << "indices (GPU MEM): [";
        for (int cnt = 0; cnt < indices_buffer_view.byteLength / sizeof(unsigned short); cnt++) {
            std::cout << *iptr << ", ";
            iptr++;
        }

        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

        std::cout << std::endl;

        glBindVertexArray(0);

        std::cout << "Primitive: VAO: " << vao << " Write mode: " << prim.mode << " Vertex Number: " << indices_accessor.count << std::endl;

        primitives.push_back(std::tuple(vao, prim.mode, indices_accessor.count));
        std::cout << "Comp type: " << indices_accessor.componentType << std::endl;
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
        //std::cout << "Drawing with: ";
        //print_tuple(prim);
        glBindVertexArray(std::get<0>(prim));

        /*unsigned short* iptr = (unsigned short*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short), GL_MAP_READ_BIT);
        float* vptr = (float *)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float), GL_MAP_READ_BIT);

        std::cout << "Generated pointers" << std::endl;

        std::cout << "First indice: " << *iptr << std::endl << "First Vertex Info: " << *vptr << std::endl;

        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        glUnmapBuffer(GL_ARRAY_BUFFER);*/

        glDrawElements(std::get<1>(prim), std::get<2>(prim), GL_UNSIGNED_SHORT, (void*)0);
        glBindVertexArray(0);
    }
}
