#version 330 core

layout(std140) uniform Light {
    vec3 lightPos;

    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 normals;

uniform sampler2D ColorMap;
uniform sampler2D RoughtnessMap;

uniform vec3 viewPos;


void main() {
    // normalized vectors
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(normals);
    // ambient
    vec3 ambient = lightAmbient * texture(ColorMap, TexCoords).rgb;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = lightSpecular * spec * texture(RoughtnessMap, TexCoords).rgb;  
        
    vec3 result = ambient + specular;
    FragColor = vec4(result, 1.0);
}