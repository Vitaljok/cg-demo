#pragma once

#include "structs.hpp"
#include <array>
#include <map>

struct GLFWwindow;

struct GUIData {
  float fov;
  bool wireframeMode = false;
  std::map<std::string, Material *> materials;
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