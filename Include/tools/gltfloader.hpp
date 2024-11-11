#ifndef GLTF_H
#define GLTF_H

#include <optional>

#include <tiny_gltf.h>
#include <glad/glad.h>

class GltfMaterial {
    public:
        GltfMaterial(tinygltf::Model &root, tinygltf::Material mat);
        GltfMaterial() {};
        void activate() const;

    private:
        GLuint basecolor_gputex;
        GLuint metallic_roughness_gputex;
};

class GltfPrimitive {
    public:
        GltfPrimitive(tinygltf::Model &root, const tinygltf::Primitive &prim);
        void draw() const;
    
    private:
        GLuint vao;
        int draw_mode;
        size_t vertex_count;
        GltfMaterial material;
};

class GltfMesh {
    public:
        GltfMesh(tinygltf::Model &root, tinygltf::Mesh mesh);
        void draw();

    private:
        std::vector<GltfPrimitive> primitives;
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