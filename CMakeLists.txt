cmake_minimum_required(VERSION 3.15)
project(Informatikprojekt C)

set(CMAKE_C_STANDARD 11)

add_executable(Informatikprojekt main.c particlesystem.h particlesystem.c init.h initOpenGL.h initOpenGL.c glad/src/glad.c)

# GLFW
add_subdirectory(./glfw-3.3)
include_directories(./glfw-3.3/include)
target_link_libraries(Informatikprojekt OpenGL32 glfw ${GLFW_LIBRARIES})

# glad
include_directories(./glad/include)


