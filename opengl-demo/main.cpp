#include <glad/gl.h>
// glad before glfw
#include <GLFW/glfw3.h>

#include <assert.h>
#include <print>
#include <iostream>
#include <demo/utils.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

#include "shader.hpp"
#include "texture.hpp"

void windowResizeCb(GLFWwindow *window [[maybe_unused]], int width,
                    int height) {
  glViewport(0, 0, width, height);
}

struct VertexData {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 uv;
};

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
  ShaderProgram shaderProgram(
      {Shader(GL_VERTEX_SHADER, "assets/shaders/opengl-demo/demo.vert"),
       Shader(GL_FRAGMENT_SHADER, "assets/shaders/opengl-demo/demo.frag")});

  // textures
  Texture woodTexture("assets/textures/old_wood.jpg", GL_RGB);

  Texture smileTexture("assets/textures/smile_sunglasses.png", GL_RGBA);
  glBindTexture(GL_TEXTURE_2D, smileTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // geometry
  const std::vector<VertexData> vertices = {
      {{-0.5, -0.5, 0}, {1.0, 1.0, 1.0}, {0.0, 0.0}},
      {{0.5, -0.5, 0}, {1.0, 0.0, 0.0}, {1.0, 0.0}},
      {{0.5, 0.5, 0}, {0.0, 1.0, 0.0}, {1.0, 1.0}},
      {{-0.5, 0.5, 0}, {0.0, 0.0, 1.0}, {0.0, 1.0}},
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
  // location, size, type, normalize, stride, offset
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                        (void *)offsetof(VertexData, pos));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                        (void *)offsetof(VertexData, color));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                        (void *)offsetof(VertexData, uv));
  glEnableVertexAttribArray(2);

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

    // use shader
    glUseProgram(shaderProgram);

    // update uniforms
    float ts = glfwGetTime();
    float value = sin(ts) / 2.0f + 0.5f;
    glUniform1f(0, value);
    // texture unit ids
    glUniform1i(1, 0); // location=1, unit=GL_TEXTURE0
    glUniform1i(2, 1); // location=2, unit=GL_TEXTURE1
    // transform matrix
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(sin(ts) * 0.75f, 0.0f, 0.0f));
    trans = glm::scale(trans, glm::vec3(cos(ts) / 3.0 + 0.5f));
    trans = glm::rotate(trans, ts, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(trans));

    // draw
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, smileTexture);

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