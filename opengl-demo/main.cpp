#include <glad/gl.h>
// glad before glfw
#include <GLFW/glfw3.h>

#include <demo/utils.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <print>
#include <stdexcept>

#include "camera.hpp"
#include "gui.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

class OpenGLApp {
private:
  glm::ivec2 windowSize = {1200, 800};
  float fov = 45.0;
  GLFWwindow *window;
  ShaderProgram shaderProgram;
  Texture dayTexture;
  Texture nightTexture;
  Mesh mesh;
  Camera camera;
  GUI gui;
  glm::dvec2 lastCursor;
  bool cursorTracking = false;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowSize.x, windowSize.y, "OpenGL Demo",
                              nullptr, nullptr);

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

    // setup callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(
        window, [](GLFWwindow *window, int width, int height) {
          glViewport(0, 0, width, height);
          auto app =
              reinterpret_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
          app->windowSize = {width, height};
        });

    glfwSetScrollCallback(window, [](GLFWwindow *window, double dx, double dy) {
      auto app =
          reinterpret_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
      app->fov += dy;
    });
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
    camera = Camera(glm::vec3(0.0f, 20.0f, 30.0f), -90, -25);
    gui = GUI(window);

    // OpenGL config
    glViewport(0, 0, windowSize.x, windowSize.y);
    glEnable(GL_DEPTH_TEST);
  }

  void processInput(float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, true);
    }

    // WASD movement
    if (glfwGetKey(window, GLFW_KEY_A)) {
      camera.move(Left, dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
      camera.move(Right, dt);
    }
    if (glfwGetKey(window, GLFW_KEY_E)) {
      camera.move(Up, dt);
    }
    if (glfwGetKey(window, GLFW_KEY_Q)) {
      camera.move(Down, dt);
    }
    if (glfwGetKey(window, GLFW_KEY_W)) {
      camera.move(Forward, dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
      camera.move(Back, dt);
    }

    // mouse
    if (!cursorTracking &
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwGetCursorPos(window, &lastCursor.x, &lastCursor.y);
      cursorTracking = true;
    }

    if (cursorTracking &
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      cursorTracking = false;
    }

    if (cursorTracking) {
      glm::dvec2 cursor;
      glfwGetCursorPos(window, &cursor.x, &cursor.y);
      glm::dvec2 offset = cursor - lastCursor;
      lastCursor = cursor;
      camera.rotate(offset);
    }
  }

  void mainLoop() {
    float tsPrev = 0;

    while (!glfwWindowShouldClose(window)) {
      float ts = glfwGetTime();
      float dt = ts - tsPrev;
      tsPrev = ts;

      processInput(dt);

      if (gui.data.wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      gui.data.fov = fov;

      // render
      glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // use shader
      glUseProgram(shaderProgram);

      float dayNight = sin(ts / 2.0f) * 5.0f + 0.5f;
      shaderProgram.setFloat("dayNight", dayNight);

      shaderProgram.setTexture("tex1", 0, dayTexture);
      shaderProgram.setTexture("tex2", 1, nightTexture);

      glm::mat4 model = glm::mat4(1.0f);
      shaderProgram.setMatrix("model", glm::value_ptr(model));

      glm::mat4 view = camera.getViewMatrix();
      shaderProgram.setMatrix("view", glm::value_ptr(view));

      glm::mat4 projection = glm::perspective(
          glm::radians(fov), (float)windowSize.x / (float)windowSize.y, 0.1f,
          100.0f);
      shaderProgram.setMatrix("projection", glm::value_ptr(projection));

      // draw
      mesh.draw();

      gui.draw();

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