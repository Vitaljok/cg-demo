#include "camera.hpp"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

constexpr glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};

Camera::Camera(glm::vec3 position_, float yaw_, float pitch_)
    : position(position_), yaw(yaw_), pitch(pitch_) {
  update();
}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, position + front, up);
}

void Camera::move(CameraMovement dir, float dt) {
  float v = moveSpeed * dt;
  switch (dir) {
  case Forward:
    position += front * v;
    break;
  case Back:
    position -= front * v;
    break;
  case Left:
    position -= right * v;
    break;
  case Right:
    position += right * v;
    break;
  case Up:
    position += up * v;
    break;
  case Down:
    position -= up * v;
    break;
  }
}

void Camera::rotate(glm::vec2 offset) {
  yaw += offset.x * sensitivity;
  pitch = std::clamp(pitch - offset.y * sensitivity, -89.0f, 89.0f);
  update();
}

void Camera::update() {
  glm::vec3 f(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
              sin(glm::radians(pitch)),
              sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
  front = glm::normalize(f);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}
