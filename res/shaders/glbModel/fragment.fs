#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 normals;

uniform sampler2D tex;

void main() { 
    vec4 result = texture(tex, TexCoords);
    FragColor = vec4(result);
}