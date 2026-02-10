#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
	UPWARD,
	DOWNWARD
};

const float PITCH = 0.0f;
const float YAW = -90.0f;
const float MOVEMENT_SPEED = 2.5f;
const float MOVEMENT_SPEED_MULTIPLIER = 5.0f;
const float INITIAL_ZOOM = 45.0f;
const float MOUSE_SENSITIVITY = 0.1f;

const float MAXIMUM_ZOOM = 75.0f;
const float MINIMUM_ZOOM = 1.0f;

class Camera {
public:
	glm::vec3 cameraPosition;
	glm::vec3 cameraDirection;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;
	glm::vec3 worldUp;

	float pitch;
	float yaw;

	float movementSpeed;

	float zoom;

	Camera() {
		cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 3.0f);
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		pitch = 0.0f;
		yaw = 0.0f;
		movementSpeed = 0.0f;
		zoom = 0.0f;
	}

	Camera(glm::vec3 position, 
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			float pitch = PITCH, 
			float yaw = YAW, 
			float movementSpeed = MOVEMENT_SPEED,
			float zoom = INITIAL_ZOOM) {

		cameraPosition = position;
		worldUp = up;
		this->yaw = yaw;
		this->pitch = pitch;
		this->movementSpeed = movementSpeed;
		this->zoom = zoom;

		UpdateCameraVectors();
	}

	void ProcessKeyboardInput(CameraMovement dir, float deltaTime, float speedMultiplier = 1.0f) {
		float velocity = movementSpeed * deltaTime * speedMultiplier;

		if (dir == FORWARD)  cameraPosition += cameraDirection * velocity;
		if (dir == BACKWARD) cameraPosition -= cameraDirection * velocity;
		if (dir == RIGHT)    cameraPosition += cameraRight * velocity;
		if (dir == LEFT)     cameraPosition -= cameraRight * velocity;
		if (dir == UPWARD)   cameraPosition += cameraUp * velocity;
		if (dir == DOWNWARD) cameraPosition -= cameraUp * velocity;
	}

	void ProcessMouseInput(float xOffset, float yOffset, GLboolean constrainPitch = true) {
		xOffset *= MOUSE_SENSITIVITY;
		yOffset *= MOUSE_SENSITIVITY;
		yaw += xOffset;
		pitch += yOffset;

		if (constrainPitch) {
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		UpdateCameraVectors();
	}

	void ProcessMouseScroll(float yOffset) {
		zoom -= yOffset;

		if (zoom <= MINIMUM_ZOOM) {
			zoom = MINIMUM_ZOOM;
		}
		if (zoom >= MAXIMUM_ZOOM) {
			zoom = MAXIMUM_ZOOM;
		}
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(cameraPosition, (cameraPosition + cameraDirection), worldUp);
	}

private:
	void UpdateCameraVectors() {
		glm::vec3 front {};
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection = glm::normalize(front);
		cameraRight = glm::normalize(glm::cross(cameraDirection, worldUp)); 
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));
	}
};

#endif