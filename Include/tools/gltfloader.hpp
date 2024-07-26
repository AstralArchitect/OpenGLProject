#ifndef GLTF_H
#define GLTF_H

#include <optional>

#include <tiny_gltf.h>
#include <glad/glad.h>

class GltfModel {
    public:
        GltfModel loadWithPath(const char* filename);
    
    private:
        tinygltf::Model model;
        std::vector<GltfNode> children;
};

class GltfNode {
    public:
        GltfNode(tinygltf::Model &root, tinygltf::Node node);

    private:
        std::optional<GltfMesh> mesh;
        std::vector<GltfNode> children;
};

class GltfMesh {
    public:
        GltfMesh(tinygltf::Model &root, tinygltf::Mesh mesh);

    private:
        std::vector<std::tuple<GLuint, GLenum, GLsizei>> primitives;
};

#endif