#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct Material
{
public:
	float* ColorPointer() {
		return &color.r;
	}

	glm::vec4 GetColor() const {
		return color;
	}

	glm::vec3 ambient = glm::vec3(0.1f);
	glm::vec3 specular = glm::vec3(0.5f);
	glm::vec3 diffuse = glm::vec3(1.0f);
	float shininess = 32.0f;

private:
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

#endif