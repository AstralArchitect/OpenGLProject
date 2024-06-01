#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D myTexture;

void main(){
    vec3 tex = vec3(texture(myTexture, TexCoords));

    FragColor = vec4(tex, 1.0);
}