#pragma once

#include <glm/glm.hpp>

enum CameraMovement { Forward, Back, Left, Right, Up, Down };

class Camera {
public:
  Camera() = default;
  Camera(glm::vec3 position, float yaw = 0, float pitch = 0);
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 right;
  glm::vec3 up;
  float yaw;
  float pitch;
  float moveSpeed = 30.0;
  float sensitivity = 0.075;

  glm::mat4 getViewMatrix();
  void move(CameraMovement dir, float dt);
  void rotate(glm::vec2 offset);

private:
  void update();
};
