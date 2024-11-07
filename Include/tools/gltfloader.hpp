#ifndef GLTF_H
#define GLTF_H

#include <optional>

#include <tiny_gltf.h>
#include <glad/glad.h>

class GltfMesh {
    public:
        GltfMesh(tinygltf::Model &root, tinygltf::Mesh mesh);
        void draw();

    private:
        std::vector<std::tuple<GLuint, GLenum, GLsizei>> primitives;
};

class GltfNode {
    public:
        GltfNode(tinygltf::Model &root, tinygltf::Node node);
        void draw();

    private:
        std::optional<GltfMesh> mesh;
        std::vector<GltfNode> children;
};

class GltfModel {
    public:
        static GltfModel loadWithPath(const char* filename);
        void draw();

    private:
        tinygltf::Model model;
        std::vector<GltfNode> children;
};

#endif