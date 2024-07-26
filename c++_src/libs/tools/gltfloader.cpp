#include "gltfloader.hpp"

#include <iostream>

#include <glad/glad.h>

#define btogl(x) x ? GL_TRUE : GL_FALSE

GltfModel GltfModel::loadWithPath(const char* filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
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

GltfMesh::GltfMesh(tinygltf::Model &root, tinygltf::Mesh mesh) {
    for (const tinygltf::Primitive &prim : mesh.primitives) {
        GLuint vao;
        glGenVertexArrays(1, &vao);
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
            } else if (attr.first.compare("NORMAL") == 0) {
                normal_buf = std::optional(BufferInfos { accessor, buffer_view, buffer });
                stride += 3;
            } else if (attr.first.compare("TEXCOORD_0") == 0) {
                texcoord_buf = std::optional(BufferInfos { accessor, buffer_view, buffer });
                stride += 2;
            } else {
                std::cout << "[Gltf] Unsupported attributes found in model: " << attr.first << " -> Skipping !" << std::endl;
                continue;
            }
        }
        assert(pos_buf_opt.has_value());

        BufferInfos pos_buf = pos_buf_opt.value();

        glVertexAttribPointer(0, 3, GL_FLOAT, btogl(pos_buf.accessor.normalized), stride * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        if (texcoord_buf) {
            glVertexAttribPointer(1, 2, GL_FLOAT, btogl(texcoord_buf.value().accessor.normalized), stride * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        if (normal_buf) {
            glVertexAttribPointer(2, 3, GL_FLOAT, btogl(normal_buf.value().accessor.normalized), stride * sizeof(float), (void*)((stride - 3) * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        std::vector<float> final_buffer;
        for (int idx = 0; idx < pos_buf.accessor.count; idx++) {
            final_buffer.insert(
                final_buffer.end(),
                pos_buf.buffer.data.begin() + pos_buf.buffer_view.byteOffset + (idx * pos_buf.buffer_view.byteStride),
                pos_buf.buffer.data.begin() + pos_buf.buffer_view.byteOffset + (idx * pos_buf.buffer_view.byteStride) + 3
            );

            if (texcoord_buf) {
                final_buffer.insert(
                    final_buffer.end(),
                    texcoord_buf.value().buffer.data.begin() + texcoord_buf.value().buffer_view.byteOffset + (idx * texcoord_buf.value().buffer_view.byteStride),
                    texcoord_buf.value().buffer.data.begin() + texcoord_buf.value().buffer_view.byteOffset + (idx * texcoord_buf.value().buffer_view.byteStride) + 3
                );
            }

            if (normal_buf) {
                final_buffer.insert(
                    final_buffer.end(),
                    normal_buf.value().buffer.data.begin() + normal_buf.value().buffer_view.byteOffset + (idx * normal_buf.value().buffer_view.byteStride),
                    normal_buf.value().buffer.data.begin() + normal_buf.value().buffer_view.byteOffset + (idx * normal_buf.value().buffer_view.byteStride) + 3
                );
            }
        }

        glBufferData(GL_ARRAY_BUFFER, final_buffer.size() * sizeof(float), &final_buffer, GL_STATIC_DRAW);

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        assert((prim.indices >= 0) && (prim.indices < root.accessors.size()));
        tinygltf::Accessor indices_accessor = root.accessors[prim.indices];
        
        assert((indices_accessor.bufferView >= 0) && (indices_accessor.bufferView < root.bufferViews.size()));
        tinygltf::BufferView indices_buffer_view = root.bufferViews[indices_accessor.bufferView];

        assert((indices_buffer_view.buffer >= 0) && (indices_buffer_view.buffer < root.buffers.size()));
        tinygltf::Buffer indices_buffer = root.buffers[indices_buffer_view.buffer];

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer_view.byteLength, &indices_buffer.data.at(0) + indices_buffer_view.byteOffset, GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        primitives.push_back(std::tuple(vao, prim.mode, indices_accessor.count));
    }
}

/*
for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {  // TODO impl drawarrays
        std::cout << "WARN: bufferView.target is zero" << std::endl;
        continue;
    }

    const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
    std::cout << "bufferview.target " << bufferView.target << std::endl;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);

    std::cout << "buffer.data.size = " << buffer.data.size()
              << ", bufferview.byteOffset = " << bufferView.byteOffset
              << std::endl;

    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
}
*/
