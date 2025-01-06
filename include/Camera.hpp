#ifndef CAMERA_HPP
#define CAMERA_HPP
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

class Camera {

public:
  // Default Constructor
  Camera();
  glm::mat4 GetViewMatrix() const;

  void MouseLook(int mouseX, int mouseY);

  void MoveForward(float speed);
  void MoveBackward(float speed);
  void MoveLeft(float speed);
  void MoveRight(float speed);

private:
  glm::vec3 mEye;
  glm::vec3 mViewDirection;
  glm::vec3 mUpVector;
  glm::vec2 mOldMousePos;
};

#endif // !CAMERA_HPP
