add_executable(task4
  src/main.cpp
  src/camera.h
  src/shader_utils.h
  src/shaders_embedded.h
  src/mesh.h
  src/input.h
  src/state.h
)

if (WIN32)
  target_link_libraries(task4 PRIVATE glfw glad glm::glm opengl32)
else()
  target_link_libraries(task4 PRIVATE glfw glad glm::glm)
endif()
