cmake_minimum_required(VERSION 3.15)
project(Informatikprojekt C)

set(CMAKE_C_STANDARD 11)

add_executable(Informatikprojekt main.c particlesystem.h particlesystem.c)

# GLFW
add_subdirectory(./glfw-3.3)
include_directories(./glfw-3.3/include)
target_link_libraries(Informatikprojekt -lOpenGL32 glfw ${GLFW_LIBRARIES})