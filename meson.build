project('OpenGL', 'c', 'cpp', 'rust', version : 'Stable')

cppDir = 'c++_src/'
rustDir = 'rustsrc/'

personnalCppLibs = cppDir + 'libs/'

toolsDir = personnalCppLibs + 'tools/'

OpenGL_lib = 'c++_src/OpenGL_libs/'

rust_sources = [rustDir + 'lib.rs']
rustlib = static_library('rustlib', rust_sources, rust_abi: 'c')

cppSources = [cppDir + 'main.cpp']
OpenGlLibsSources = [OpenGL_lib + 'glad.c']
personnal_cpp_sources = [personnalCppLibs + 'functions.cpp']
toolsSources = [toolsDir + 'camera.cpp', toolsDir + 'shader.cpp', toolsDir + 'model.cpp']

sources = [cppSources, OpenGlLibsSources, personnal_cpp_sources, toolsSources]
include_dir = include_directories('Include')
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

cmake = import('cmake')
opt_var = cmake.subproject_options()
opt_var.add_cmake_defines({
    'TINYGLTF_BUILD_LOADER_EXAMPLE': false,
    'TINYGLTF_HEADER_ONLY': true,
    'TINYGLTF_INSTALL': false
})

tinygltf = cmake.subproject('tinygltf', options: opt_var)
tinygltf_dep = tinygltf.dependency('tinygltf')

executable('main', sources: sources, include_directories: include_dir, dependencies: [glfw_dep, glm_dep, tinygltf_dep], link_with: rustlib)
