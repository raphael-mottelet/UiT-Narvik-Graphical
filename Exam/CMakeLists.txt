cmake_minimum_required(VERSION 3.20)

# Make third-party projects (glfw/glad/glm) compatible with newer CMake
cmake_policy(VERSION 3.20...4.1)
set(CMAKE_POLICY_VERSION_MINIMUM 3.20)

project(exam LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if (APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

include(FetchContent)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLAD – core profile OpenGL 3.3
set(GLAD_PROFILE "core" CACHE STRING "" FORCE)
set(GLAD_API "gl=3.3" CACHE STRING "" FORCE)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM (header-only)
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Executable
add_executable(exam
    main.cpp
    utils/app.cpp          # new file with the game loop
    graphics/stb_image_impl.cpp
    graphics/text_renderer.cpp
)

# Include the project root so "menu/...", "utils/...", "graphics/..." etc. all work
target_include_directories(exam PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linking
if (WIN32)
    target_link_libraries(exam PRIVATE glfw glad glm::glm opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(exam PRIVATE glfw glad glm::glm OpenGL::GL)
endif()
