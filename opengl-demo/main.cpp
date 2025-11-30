#include <glad/gl.h>
// glad before glfw
#include <GLFW/glfw3.h>

#include <assert.h>
#include <print>
#include <iostream>
#include <demo/utils.hpp>
#include <glm/glm.hpp>
#include <stdexcept>
#include <vector>

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
  const int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    throw std::runtime_error("Failed to initialize OpenGL context");
  }
  std::println("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version),
               GLAD_VERSION_MINOR(version));

  // setup viewport
  glfwSetFramebufferSizeCallback(window, windowResizeCb);
  glViewport(0, 0, 1200, 800);

  // shaders
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  {
    auto source = readTextFile("assets/shaders/opengl-demo/demo.vert");
    const char *src = source.c_str();

    glShaderSource(vertexShader, 1, &src, NULL);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      std::string info(512, 0);
      glGetShaderInfoLog(vertexShader, 512, NULL, info.data());
      throw std::runtime_error(
          std::format("Vertex shader compilation failed\n{}", info));
    }
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  {
    auto source = readTextFile("assets/shaders/opengl-demo/demo.frag");
    const char *src = source.c_str();

    glShaderSource(fragmentShader, 1, &src, NULL);
    glCompileShader(fragmentShader);

    GLint success;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      std::string info(512, 0);
      glGetShaderInfoLog(fragmentShader, 512, NULL, info.data());
      throw std::runtime_error(
          std::format("Fragment shader compilation failed\n{}", info));
    }
  }

  GLuint shaderProgram = glCreateProgram();
  {
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
      std::string info(512, 0);
      glGetProgramInfoLog(shaderProgram, 512, NULL, info.data());
      throw std::runtime_error(
          std::format("Shader program linking failed\n{}", info));
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  // geometry
  const std::vector<glm::vec3> vertices = {
      {-0.5, -0.5, 0},
      {0.5, -0.5, 0},
      {0.5, 0.5, 0},
      {-0.5, 0.5, 0},
  };

  const std::vector<uint32_t> indices = {0, 1, 3, 1, 2, 3};

  // VAO - Vertex Array Object
  // Stores vertex attribute layouts and memory buffers to read them from
  GLuint vertexArray;
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  // VBO - Vertex Buffer Object
  // memory buffer, vertex attributes
  GLuint vertexBuf;
  glGenBuffers(1, &vertexBuf);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  // EBO - Element Buffer Object
  // memory buffer, vertex indices
  GLuint indexBuf;
  glGenBuffers(1, &indexBuf);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(),
               indices.data(), GL_STATIC_DRAW);

  // Attribute layout
  // location=0, size=3, type=float32, normalize=false, stride=3*float32, offset=0
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window)) {
    // input
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, true);
    }

    // render
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram(shaderProgram);
    glBindVertexArray(vertexArray);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
    
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