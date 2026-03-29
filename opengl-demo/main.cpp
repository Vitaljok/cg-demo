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

struct Material {
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};

struct Light {
  glm::vec3 pos;
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

class OpenGLApp {
private:
  glm::ivec2 windowSize = {1200, 800};
  float fov = 45.0;
  GLFWwindow *window;
  ShaderProgram shaderProgram;
  ShaderProgram lampProgram;

  Mesh sphere;
  glm::mat4 sphereMatrix;
  glm::mat4 lampMatrix;
  Mesh cube;
  glm::mat4 cubeMatrix;
  Mesh monkey;
  glm::mat4 monkeyMatrix;
  Mesh plane;
  glm::mat4 planeMatrix;

  Material material = {.diffuse = {0.9f, 0.5f, 0.3f},
                       .specular = {1.0f, 1.0f, 1.0f},
                       .shininess = 32};

  Light light = {.pos = {3.0f, 5.0f, 5.0f},
                 .ambient = {0.1f, 0.1f, 0.1f},
                 .diffuse = {1.0f, 1.0f, 1.0f},
                 .specular = {0.5f, 0.5f, 0.5f}};

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
    shaderProgram = ShaderProgram("assets/shaders/opengl-demo/shading.vert",
                                  "assets/shaders/opengl-demo/shading.frag");

    lampProgram = ShaderProgram("assets/shaders/opengl-demo/shading.vert",
                                "assets/shaders/opengl-demo/shading_lamp.frag");

    {
      cube = Mesh("assets/meshes/cube.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, {-2.0f, 1.0f, -1.0f});
      mat = glm::rotate(mat, glm::radians(35.0f), {0.0f, 1.0f, 0.0f});
      cubeMatrix = mat;
    }

    {
      sphere = Mesh("assets/meshes/icosphere.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, {2.0f, 1.0f, -1.0f});
      sphereMatrix = mat;
    }

    {
      monkey = Mesh("assets/meshes/suzanne.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, {0.0f, 1.0f, 2.0f});
      monkeyMatrix = mat;
    }

    {
      plane = Mesh("assets/meshes/plane.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::scale(mat, {5.0f, 5.0f, 5.0f});
      planeMatrix = mat;
    }

    {
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, light.pos);
      mat = glm::scale(mat, {0.2f, 0.2f, 0.2f});
      lampMatrix = mat;
    }
  }

  void init() {
    initWindow();
    loadAssets();
    camera = Camera(glm::vec3(0.0f, 5.0f, 12.0f), -90, -20);

    gui = GUI(window);
    gui.data.materialDiffuse = glm::value_ptr(material.diffuse);
    gui.data.materialSpecular = glm::value_ptr(material.specular);
    gui.data.materialShininess = &material.shininess;
    gui.data.lightAmbient = glm::value_ptr(light.ambient);
    gui.data.lightDiffuse = glm::value_ptr(light.diffuse);
    gui.data.lightSpecular = glm::value_ptr(light.specular);

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

  void processGUI() {
    if (gui.data.wireframeMode) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    gui.data.fov = fov;
  }

  void mainLoop() {
    float tsPrev = 0;

    while (!glfwWindowShouldClose(window)) {
      float ts = glfwGetTime();
      float dt = ts - tsPrev;
      tsPrev = ts;

      processInput(dt);
      processGUI();

      glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(shaderProgram);

      glm::mat4 view = camera.getViewMatrix();
      shaderProgram.setMatrix("view", glm::value_ptr(view));

      glm::mat4 projection = glm::perspective(
          glm::radians(fov), (float)windowSize.x / (float)windowSize.y, 0.1f,
          100.0f);
      shaderProgram.setMatrix("projection", glm::value_ptr(projection));

      shaderProgram.setVec3("material.diffuse",
                            glm::value_ptr(material.diffuse));
      shaderProgram.setVec3("material.specular",
                            glm::value_ptr(material.specular));
      shaderProgram.setFloat("material.shininess", material.shininess);

      shaderProgram.setVec3("light.pos", glm::value_ptr(light.pos));
      shaderProgram.setVec3("light.ambient", glm::value_ptr(light.ambient));
      shaderProgram.setVec3("light.diffuse", glm::value_ptr(light.diffuse));
      shaderProgram.setVec3("light.specular", glm::value_ptr(light.specular));

      shaderProgram.setVec3("cameraPos", glm::value_ptr(camera.position));

      shaderProgram.setMatrix("model", glm::value_ptr(monkeyMatrix));
      monkey.draw();
      shaderProgram.setMatrix("model", glm::value_ptr(cubeMatrix));
      cube.draw();
      shaderProgram.setMatrix("model", glm::value_ptr(sphereMatrix));
      sphere.draw();
      shaderProgram.setMatrix("model", glm::value_ptr(planeMatrix));
      plane.draw();

      glUseProgram(lampProgram);
      lampProgram.setMatrix("model", glm::value_ptr(lampMatrix));
      lampProgram.setMatrix("view", glm::value_ptr(view));
      lampProgram.setMatrix("projection", glm::value_ptr(projection));
      lampProgram.setVec3("lightColor", glm::value_ptr(light.diffuse));
      sphere.draw();

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