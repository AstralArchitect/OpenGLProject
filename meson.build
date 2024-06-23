project('learnOpenGL', 'c', 'cpp', 'rust')

cppDir = 'c++_src/'
rustDir = 'rustsrc/'

personnalCppLibs = cppDir + 'libs/'

OpenGL_lib = 'c++_src/OpenGL_libs/'

rust_sources = [rustDir + 'lib.rs']
# rustlib = static_library('rustlib', rust_sources, rust_abi: 'c')
rustlib = static_library('rustlib', rust_sources, rust_crate_type: 'staticlib')

sources = [cppDir + 'main.cpp', OpenGL_lib + 'glad.c', personnalCppLibs + 'functions.cpp']
include_dir = include_directories('Include')
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

executable('main', sources: sources, include_directories: include_dir, dependencies: [glfw_dep, glm_dep], link_with: rustlib)