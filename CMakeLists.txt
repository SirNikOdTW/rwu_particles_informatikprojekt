cmake_minimum_required(VERSION 3.15)
project(Informatikprojekt C)

set(CMAKE_C_STANDARD 11)
set(PROJECT_SOURCES particlesystem.h particlesystem.c init.h initOpenGL.h initOpenGL.c glad/src/glad.c)

add_executable(Informatikprojekt cpuMain.c ${PROJECT_SOURCES})
add_executable(Informatikprojekt_OpenGL openglMain.c ${PROJECT_SOURCES})
add_executable(Informatikprojekt_Vulkan vulkanMain.c ${PROJECT_SOURCES})

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})


# GLFW
include_directories(./glfw/include)
target_link_libraries(Informatikprojekt ${CMAKE_SOURCE_DIR}/glfw/lib-mingw-w64/libglfw3.a)
target_link_libraries(Informatikprojekt_OpenGL ${CMAKE_SOURCE_DIR}/glfw/lib-mingw-w64/libglfw3.a)
target_link_libraries(Informatikprojekt_Vulkan Vulkan::Vulkan ${CMAKE_SOURCE_DIR}/glfw/lib-mingw-w64/libglfw3.a)

# glad
include_directories(./glad/include)


