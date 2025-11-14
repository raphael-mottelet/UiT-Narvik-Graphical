cmake_minimum_required(VERSION 3.20)

# Make third-party projects (glfw/glad/glm) happy under CMake 4.x
cmake_policy(VERSION 3.5...4.1)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(GraphicsPipeline LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

if(APPLE)
  add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

include(FetchContent)

#GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

#GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM 
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Task (modular for each src)

add_subdirectory(task1)
add_subdirectory(task2)
# add_subdirectory(task3) not needed cuz only text
add_subdirectory(task4)
# add_subdirectory(task5) Paper recap
add_subdirectory(task6)
add_subdirectory(task7)
add_subdirectory(task8)


