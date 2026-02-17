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

private:
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

#endif