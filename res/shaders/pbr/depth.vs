#version 330 core

layout (location = 0) in vec3 aPos;

#if defined(HAS_BASE_COLOR_TEX) || defined(HAS_PBR_TEX)
layout (location = 1) in vec2 aTexCoords;
#endif

#ifdef HAS_NORMALS
layout (location = 2) in vec3 aNormal;
#endif

uniform mat4 transform;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = lightSpaceMatrix * transform *  vec4(aPos, 1.0);
}