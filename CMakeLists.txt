cmake_minimum_required(VERSION 3.10)
project(Informatikprojekt C)

set(CMAKE_C_STANDARD 11)
set(PROJECT_SOURCES particlesystem.h particlesystem.c utils.h utils.c)
set(PROJECT_SOURCES_OPENGL ${PROJECT_SOURCES} initOpenGL.h initOpenGL.c utils.h glad/src/glad.c)
set(PROJECT_SOURCES_VULKAN ${PROJECT_SOURCES} initVulkan.h initVulkan.c)

add_executable(Informatikprojekt cpuMain.c ${PROJECT_SOURCES_OPENGL})
add_executable(Informatikprojekt_OpenGL openglMain.c ${PROJECT_SOURCES_OPENGL})
add_executable(Informatikprojekt_Vulkan vulkanMain.c ${PROJECT_SOURCES_VULKAN})

# Copy shader file to output directory
## OpenGL
configure_file(./shaders/opengl/ComputeShader.glsl ./opengl/ComputeShader.glsl COPYONLY)
configure_file(./shaders/opengl/VertexShader.glsl ./opengl/VertexShader.glsl COPYONLY)
configure_file(./shaders/opengl/FragmentShader.glsl ./opengl/FragmentShader.glsl COPYONLY)
## Vulkan
configure_file(./shaders/vulkan/ComputeShader.comp ./vulkan/ComputeShader.comp COPYONLY)
configure_file("shaders/vulkan/VertexShader.vert" ./vulkan/VertexShader.vert COPYONLY)
configure_file("shaders/vulkan/FragmentShader.frag" ./vulkan/FragmentShader.frag COPYONLY)
configure_file(shaders/vulkan/runCompiler.sh ./vulkan/runCompiler.sh COPYONLY)
execute_process(COMMAND sh runCompiler.sh WORKING_DIRECTORY ./vulkan)

# Vulkan
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

# glad
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(./glad/include)

# Linking
target_link_libraries(Informatikprojekt OpenGL::GL glfw)
target_link_libraries(Informatikprojekt_OpenGL OpenGL::GL glfw)
target_link_libraries(Informatikprojekt_Vulkan Vulkan::Vulkan glfw)


