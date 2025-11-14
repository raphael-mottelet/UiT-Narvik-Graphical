add_executable(task6
  src/main.cpp
  src/camera.h
  src/shader_utils.h
  src/shaders_embedded.h
  src/mesh.h
  src/texture.h
  src/input.h
  src/state.h
)

if (WIN32)
  target_link_libraries(task6 PRIVATE glfw glad glm::glm opengl32)
else()
  target_link_libraries(task6 PRIVATE glfw glad glm::glm)
endif()
