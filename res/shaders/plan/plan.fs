#version 330 core

#define SHININESS 32

struct PointLight {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;
	
    vec3 diffuse;
    vec3 specular;
};
// light
uniform PointLight light;
uniform DirLight dirLight;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D colorMap;
uniform sampler2D specMap;

uniform vec3 viewPos;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main() {
    // normalized vectors
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    // calculate light    
    vec3 result = calcPointLight(light, norm, FragPos, viewDir);
    result += calcDirLight(dirLight, norm, viewDir);
    result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a point light.
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), SHININESS);
    // attenuation
    float distance = length(light.pos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(colorMap, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(colorMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specMap, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a directional light.
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), SHININESS);
    // combine results
    vec3 diffuse = light.diffuse * diff * vec3(texture(colorMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specMap, TexCoords));
    return (diffuse + specular);
}