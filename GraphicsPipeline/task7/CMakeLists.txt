add_executable(task7
  src/main.cpp
  src/camera.h
  src/input.h
  src/mesh.h
  src/shader_utils.h
  src/shaders_embedded.h
  src/state.h
)

if (WIN32)
  target_link_libraries(task7 PRIVATE glfw glad glm::glm opengl32)
else()
  target_link_libraries(task7 PRIVATE glfw glad glm::glm)
endif()
