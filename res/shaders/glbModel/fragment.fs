#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D tex;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 ambientColor;

void main()
{           
    vec3 color = texture(tex, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = ambientColor * color;
    
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}