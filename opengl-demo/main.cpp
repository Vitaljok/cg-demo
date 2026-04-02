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
#include "structs.hpp"
#include "texture.hpp"

class OpenGLApp {
private:
  glm::ivec2 windowSize = {1200, 800};
  float fov = 45.0;
  GLFWwindow *window;
  ShaderProgram shaderProgram;
  ShaderProgram lampProgram;

  Mesh sphere;
  glm::mat4 sphereMatrix;
  Material sphereMaterial;
  glm::mat4 lampMatrix;

  Mesh cube;
  glm::mat4 cubeMatrix;
  Material cubeMaterial;

  Mesh monkey;
  glm::mat4 monkeyMatrix;
  Material monkeyMaterial;

  Mesh plane;
  glm::mat4 planeMatrix;
  Material planeMaterial;

  Mesh barrel;
  glm::mat4 barrelMatrix;
  Material barrelMaterial;

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
      cubeMaterial = {
          .diffuse = Texture("assets/textures/box_diffuse.png", GL_RGB),
          .specular = Texture("assets/textures/box_specular.png", GL_RGB),
          .shininess = 2};
    }

    {
      sphere = Mesh("assets/meshes/icosphere.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, {2.0f, 2.5f, -1.0f});
      mat = glm::scale(mat, {0.5f, 0.5f, 0.5f});
      sphereMatrix = mat;
      sphereMaterial = {
          .diffuse = Texture("assets/textures/sphere_diffuse.png", GL_RGB),
          .specular = Texture("assets/textures/sphere_specular.png", GL_RGB),
          .shininess = 128};
    }

    {
      monkey = Mesh("assets/meshes/suzanne.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, {0.0f, 1.0f, 2.0f});
      monkeyMatrix = mat;
      monkeyMaterial = {
          .diffuse = Texture("assets/textures/monkey_diffuse.png", GL_RGB),
          .specular = Texture("assets/textures/monkey_specular.png", GL_RGB),
          .shininess = 8};
    }

    {
      plane = Mesh("assets/meshes/plane.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::scale(mat, {5.0f, 5.0f, 5.0f});
      planeMatrix = mat;
      planeMaterial = {
          .diffuse = Texture("assets/textures/plane_diffuse.png", GL_RGB),
          .specular = Texture("assets/textures/plane_specular.png", GL_RGB),
          .shininess = 32};
    }

    {
      barrel = Mesh("assets/meshes/barrel.obj");
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, {2.0f, 0.0f, -1.0f});
      barrelMatrix = mat;
      barrelMaterial = {
        .diffuse = Texture("assets/textures/barrel_diffuse.png", GL_RGB),
        .specular = Texture("assets/textures/barrel_specular.png", GL_RGB),
        .shininess = 2};
    }

    {
      glm::mat4 mat = glm::mat4(1.0f);
      mat = glm::translate(mat, light.pos);
      mat = glm::scale(mat, {0.2f, 0.2f, 0.2f});
      lampMatrix = mat;
    }
  }

  void initGUI() {
    gui = GUI(window);
    gui.data.materials.emplace("Cube", &cubeMaterial);
    gui.data.materials.emplace("Shpere", &sphereMaterial);
    gui.data.materials.emplace("Monkey", &monkeyMaterial);
    gui.data.materials.emplace("Barrel", &barrelMaterial);
    gui.data.materials.emplace("Plane", &planeMaterial);

    gui.data.lightAmbient = glm::value_ptr(light.ambient);
    gui.data.lightDiffuse = glm::value_ptr(light.diffuse);
    gui.data.lightSpecular = glm::value_ptr(light.specular);
  }

  void init() {
    initWindow();
    loadAssets();
    initGUI();
    camera = Camera(glm::vec3(0.0f, 5.0f, 12.0f), -90, -20);

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

      light.use(shaderProgram);

      shaderProgram.setVec3("cameraPos", glm::value_ptr(camera.position));

      shaderProgram.setMatrix("model", glm::value_ptr(monkeyMatrix));
      monkeyMaterial.use(shaderProgram);
      monkey.draw();

      shaderProgram.setMatrix("model", glm::value_ptr(cubeMatrix));
      cubeMaterial.use(shaderProgram);
      cube.draw();

      shaderProgram.setMatrix("model", glm::value_ptr(sphereMatrix));
      sphereMaterial.use(shaderProgram);
      sphere.draw();

      shaderProgram.setMatrix("model", glm::value_ptr(planeMatrix));
      planeMaterial.use(shaderProgram);
      plane.draw();

      shaderProgram.setMatrix("model", glm::value_ptr(barrelMatrix));
      barrelMaterial.use(shaderProgram);
      barrel.draw();

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