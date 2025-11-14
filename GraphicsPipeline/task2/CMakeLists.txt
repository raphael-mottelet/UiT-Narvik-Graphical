add_executable(task2
  src/main.cpp
  src/camera.h
  src/shader_utils.h
  src/mesh.h
  src/input.h
  src/state.h
  src/shaders_embedded.h
)

# Link libraries (same spirit as task1)
if (WIN32)
  target_link_libraries(task2 PRIVATE glfw glad glm::glm opengl32)
else()
  target_link_libraries(task2 PRIVATE glfw glad glm::glm)
endif()
