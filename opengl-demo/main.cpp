#include <glad/gl.h>
// glad before glfw
#include <GLFW/glfw3.h>

#include <assert.h>
#include <print>
#include <iostream>
#include <array>
#include <demo/utils.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

#include "shader.hpp"
#include "texture.hpp"

int windowWidth = 1200;
int windowHeight = 800;

void windowResizeCb(GLFWwindow *window [[maybe_unused]], int width,
                    int height) {
  glViewport(0, 0, width, height);
  windowWidth = width;
  windowHeight = height;
}

struct VertexData {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 uv;
};

enum Uniforms {
  value = 0,
  texture0 = 1,
  texture1 = 2,
  model = 3,
  view = 4,
  projection = 5,
};

void runOpenGLDemo() {
  // Initialize window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight,
                                        "OpenGL Demo", nullptr, nullptr);

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

  // setup
  glfwSetFramebufferSizeCallback(window, windowResizeCb);
  glViewport(0, 0, windowWidth, windowHeight);
  glEnable(GL_DEPTH_TEST);

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
      {{-0.5, +0.0, -0.5}, {1.0, 1.0, 1.0}, {0.0, 0.0}},
      {{+0.5, +0.0, +0.5}, {1.0, 0.0, 0.0}, {1.0, 0.0}},
      {{-0.5, +1.0, +0.5}, {0.0, 1.0, 0.0}, {1.0, 1.0}},
      {{+0.5, +1.0, -0.5}, {0.0, 0.0, 1.0}, {0.0, 1.0}},
  };

  const std::vector<uint32_t> indices = {0, 1, 2, 0, 3, 2, 0, 3, 1, 2, 3, 1};

  std::array objPositions = {
      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 2.0f, -5.0f),
      glm::vec3(+3.0f, -2.0f, -5.0f), glm::vec3(-3.0f, -1.0f, -10.0f),
      glm::vec3(+5.0f, 3.0f, -10.0f)};

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // use shader
    glUseProgram(shaderProgram);

    // uniforms
    float ts = glfwGetTime();
    float value = sin(ts) / 2.0f + 0.5f;
    glUniform1f(Uniforms::value, value);

    // textures
    glUniform1i(Uniforms::texture0, 0); // unit id=0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);

    glUniform1i(Uniforms::texture1, 1); // unit id=1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, smileTexture);

    // transform matrices
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -0.5f, -3.0f));
    glUniformMatrix4fv(Uniforms::view, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f,
        100.0f);
    glUniformMatrix4fv(Uniforms::projection, 1, GL_FALSE,
                       glm::value_ptr(projection));

    // draw
    glBindVertexArray(vertexArray);

    for (size_t i = 0; i < objPositions.size(); i++) {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, objPositions[i]);
      model = glm::rotate(model, ts + glm::radians(i * 20.0f),
                          glm::vec3(0.0f, 1.0f, 0.0f));
      glUniformMatrix4fv(Uniforms::model, 1, GL_FALSE, glm::value_ptr(model));
      glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
    }

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