#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D tex;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 ambientColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light’s perspective
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(float x = -2; x <= 2; x += .2)
    {
        for(float y = -2; y <= 2; y += .2)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= (20.0 * 20.0);

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

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

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    FragColor = vec4(ambient + (1.0 - shadow) * (diffuse + specular), 1.0);
}