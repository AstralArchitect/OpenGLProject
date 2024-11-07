#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 normals;

uniform sampler2D tex;

void main() { 
    vec3 result = texture(tex, TexCoords).rgb;
    result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}