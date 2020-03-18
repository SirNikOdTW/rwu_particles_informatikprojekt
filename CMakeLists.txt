cmake_minimum_required(VERSION 3.15)
project(Informatikprojekt C)

set(CMAKE_C_STANDARD 11)
set(PROJECT_SOURCES particlesystem.h particlesystem.c utils.h utils.c)
set(PROJECT_SOURCES_OPENGL ${PROJECT_SOURCES} initOpenGL.h initOpenGL.c utils.h glad/src/glad.c)
set(PROJECT_SOURCES_VULKAN ${PROJECT_SOURCES} initVulkan.h initVulkan.c)

add_executable(Informatikprojekt cpuMain.c ${PROJECT_SOURCES_OPENGL})
add_executable(Informatikprojekt_OpenGL openglMain.c ${PROJECT_SOURCES_OPENGL})
add_executable(Informatikprojekt_Vulkan vulkanMain.c ${PROJECT_SOURCES_VULKAN})

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


