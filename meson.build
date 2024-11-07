project('OpenGL', 'c', 'cpp', version : 'Alpha', default_options: ['cpp_std=c++20'])

cppDir = 'src/'

personnalCppLibs = cppDir + 'libs/'

toolsDir = cppDir + 'tools/'

OpenGL_lib = cppDir + 'OpenGL_libs/'

cppSources = [cppDir + 'main.cpp']
OpenGlLibsSources = [OpenGL_lib + 'glad.c']
personnal_cpp_sources = [cppDir + 'callbacks.cpp']
toolsSources = [toolsDir + 'camera.cpp', toolsDir + 'shader.cpp', toolsDir + 'gltfloader.cpp']

sources = [cppSources, OpenGlLibsSources, personnal_cpp_sources, toolsSources]
include_dir = include_directories('Include')
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

cmake = import('cmake')
opt_var = cmake.subproject_options()
opt_var.add_cmake_defines({
    'TINYGLTF_BUILD_LOADER_EXAMPLE': false,
    'TINYGLTF_HEADER_ONLY': false,
    'TINYGLTF_INSTALL': false
})

tinygltf = cmake.subproject('tinygltf', options: opt_var)
tinygltf_dep = tinygltf.dependency('tinygltf')

executable('main', sources: sources, include_directories: include_dir, dependencies: [glfw_dep, glm_dep, tinygltf_dep])
