project('learnOpenGL', 'c', 'cpp', 'rust')

sources = ['main.cpp', 'lib/glad.c', 'lib/functions.cpp']
include_dir = include_directories('Include')
glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')

executable('main', sources: sources, include_directories: include_dir, dependencies: [glfw_dep, glm_dep])