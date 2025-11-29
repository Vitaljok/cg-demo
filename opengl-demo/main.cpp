#include <glad/gl.h>
// glad before glfw
#include <GLFW/glfw3.h>

#include <assert.h>
#include <print>
#include <iostream>
#include <stdexcept>

void windowResizeCb(GLFWwindow *window [[maybe_unused]], int width,
                    int height) {
  glViewport(0, 0, width, height);
}

void runOpenGLDemo() {
  // Initialize window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(1200, 800, "OpenGL Demo", nullptr, nullptr);

  if (window == nullptr) {
    throw std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(window);

  // Load OpenGL
  int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    throw std::runtime_error("Failed to initialize OpenGL context");
  }
  std::println("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version),
               GLAD_VERSION_MINOR(version));

  // setup
  glfwSetFramebufferSizeCallback(window, windowResizeCb);
  glViewport(0, 0, 1200, 800);

  while (!glfwWindowShouldClose(window)) {
    // input
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, true);
    }

    // render
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // swap and events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}

int main() {
  try {
    runOpenGLDemo();
  } catch (std::exception &ex) {
    std::println("EXCEPTION: {}", ex.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}