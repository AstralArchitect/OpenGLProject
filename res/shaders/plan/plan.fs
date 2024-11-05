#version 330 core

// light variables
vec3 lightPos;
vec3 lightAmbient;
vec3 lightDiffuse;
vec3 lightSpecular;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D ColorMap;
uniform sampler2D specMap;

uniform vec3 viewPos;


void main() {
    // normalized vectors
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    // ambient
    vec3 ambient = lightAmbient * texture(ColorMap, TexCoords).rgb;

    // diffuse
    vec3 diffuse = lightDiffuse * texture(ColorMap, TexCoords).rgb;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = lightSpecular * spec * texture(specMap, TexCoords).rgb;  
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(diffuse, 1.0);
}