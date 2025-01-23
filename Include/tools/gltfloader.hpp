#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include <optional>

#include <tiny_gltf.h>
#include <glad/glad.h>
#include <tools/shader.hpp>

class GltfMaterial {
    public:
        GltfMaterial(tinygltf::Model &root, tinygltf::Material mat);
        GltfMaterial() {};
        void activate(Shader &shader) const;

    private:
        std::optional<GLuint> basecolor_gputex;
        std::optional<GLuint> metallic_roughness_gputex;
        double basecolor[3];
        double metallic_factor;
        double roughness_factor;
};

class GltfPrimitive {
    public:
        GltfPrimitive(tinygltf::Model &root, const tinygltf::Primitive &prim);
        void draw(Shader &shader) const;
        void drawWithoutTextures() const;
    
    private:
        GLuint vao;
        int draw_mode;
        size_t vertex_count;
        GltfMaterial material;
};

class GltfMesh {
    public:
        GltfMesh(tinygltf::Model &root, tinygltf::Mesh mesh);
        void draw(Shader &shader);
        void drawWithoutTextures();

    private:
        std::vector<GltfPrimitive> primitives;
};

class GltfNode {
    public:
        GltfNode(tinygltf::Model &root, tinygltf::Node node);
        void draw(Shader &shader);
        void drawWithoutTextures();

    private:
        std::optional<GltfMesh> mesh;
        std::vector<GltfNode> children;
};

class GltfModel {
    public:
        GltfModel(const char* filename);
        // TODO: créer un destructeur pour cette class
        void draw(Shader &shader);
        void drawWithoutTextures();

    private:
        tinygltf::Model tiny_model;
        std::vector<GltfNode> children;
};

#endif