#pragma once

#include <array>

struct GLFWwindow;

struct GUIData {
  float fov;
  bool wireframeMode = false;
  float *materialDiffuse;
  float *materialSpecular;
  float *materialShininess;
  float *lightAmbient;
  float *lightDiffuse;
  float *lightSpecular;
};

class GUI {
public:
  GUIData data;
  GUI() = default;
  GUI(GLFWwindow *window);

  void draw();

private:
  bool visible;
  std::array<float, 300> hist = {0};
};