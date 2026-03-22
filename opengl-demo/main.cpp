#include <glad/gl.h>
// glad before glfw
#include <GLFW/glfw3.h>

#include <array>
#include <demo/utils.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>
#include <stdexcept>

#include "mesh.hpp"
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

class OpenGLApp {
private:
  GLFWwindow *window;
  ShaderProgram shaderProgram;
  Texture dayTexture;
  Texture nightTexture;
  Mesh mesh;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Demo", nullptr,
                              nullptr);

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
  }

  void loadAssets() {
    shaderProgram = ShaderProgram("assets/shaders/opengl-demo/demo.vert",
                                  "assets/shaders/opengl-demo/demo.frag");
    dayTexture = Texture("assets/textures/keep_baked_day.png", GL_RGB);
    nightTexture = Texture("assets/textures/keep_baked_night.png", GL_RGB);
    mesh = Mesh("assets/meshes/keep.obj");
  }

  void init() {
    initWindow();
    loadAssets();
    glEnable(GL_DEPTH_TEST);
  }

  void mainLoop() {
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

      float ts = glfwGetTime();
      float dayNight = sin(ts / 2.0f) * 5.0f + 0.5f;
      shaderProgram.setFloat("dayNight", dayNight);

      shaderProgram.setTexture("tex1", 0, dayTexture);
      shaderProgram.setTexture("tex2", 1, nightTexture);

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::rotate(model, ts, glm::vec3(0.0f, 1.0f, 0.0f));
      shaderProgram.setMatrix("model", glm::value_ptr(model));

      glm::mat4 view =
          glm::lookAt(glm::vec3(0.0f, 20.0f, 30.0f),
                      glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      shaderProgram.setMatrix("view", glm::value_ptr(view));

      glm::mat4 projection = glm::perspective(
          glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f,
          100.0f);
      shaderProgram.setMatrix("projection", glm::value_ptr(projection));

      // draw
      mesh.draw();

      // swap and events
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }
  void cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

public:
  void run() {
    init();
    mainLoop();
    cleanup();
  }
};

int main() {
  try {
    OpenGLApp app;
    app.run();
  } catch (std::exception &ex) {
    std::println("EXCEPTION: {}", ex.what());
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}