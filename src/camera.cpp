#include "Camera.hpp"

Camera::Camera() {
  // Assume we are placed at origin
  mEye = glm::vec3(0.0f, 0.0f, 0.0f);
  // Assume we are looking out into the world
  mViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
  // Assume we start on perfect plane
  mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(mEye, mEye + mViewDirection, mUpVector);
}

void Camera::SetProjectionMatrix(float fovy, float aspect, float near,
                                 float far) {
  mProjectionMatrix = glm::perspective(fovy, aspect, near, far);
}

glm::mat4 Camera::GetProjectionMatrix() const { return mProjectionMatrix; }

void Camera::MouseLook(int mouseX, int mouseY) {
  std::cout << "mouse: " << mouseX << ", " << mouseY << std::endl;
  static const float sensitivity = 0.05f;
  glm::vec2 currMouse = glm::vec2(mouseX, mouseY);

  static bool firstLook = true;
  if (firstLook) {
    mOldMousePos = currMouse;
    firstLook = false;
  }

  glm::vec2 mouseDelta = mOldMousePos - currMouse;
  mouseDelta *= sensitivity;
  mViewDirection =
      glm::rotate(mViewDirection, glm::radians(mouseDelta.x), mUpVector);
  // glm::vec3 right = glm::cross(mViewDirection, mUpVector);
  // mViewDirection =
  //     glm::rotate(mViewDirection, glm::radians(-mouseDelta.y), right);
}

void Camera::MoveForward(float speed) {
  // Simple but not correct
  mEye += (mViewDirection * speed);
}
void Camera::MoveBackward(float speed) { mEye -= (mViewDirection * speed); }
void Camera::MoveLeft(float speed) {}
void Camera::MoveRight(float speed) {}
