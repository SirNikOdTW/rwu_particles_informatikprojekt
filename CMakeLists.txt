cmake_minimum_required(VERSION 3.15)
project(Informatikprojekt C)

set(CMAKE_C_STANDARD 11)

add_executable(Informatikprojekt main.c particlesystem.h particlesystem.c init.h initOpenGL.h initOpenGL.c glad/src/glad.c)

# GLFW

include_directories(./glfw/include)
target_link_libraries(Informatikprojekt ${CMAKE_SOURCE_DIR}/glfw/lib-mingw-w64/libglfw3.a)

# glad
include_directories(./glad/include)


