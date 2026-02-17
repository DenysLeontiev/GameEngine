#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform {
public:

	float* PositionPointer() {
		return &position.x;
	}

	float* RotationPointer() {
		return &rotation.x;
	}

	float* ScalePointer() {
		return &scale.x;
	}

	glm::vec3 GetPosition() const {
		return position;
	}

	glm::vec3 GetRotation() const {
		return rotation;
	}

	glm::vec3 GetScale() const {
		return scale;
	}

	glm::mat4 GetModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));

		return model;
	}

private:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

#endif