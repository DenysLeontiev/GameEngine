#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

enum LightType {
	Point
};

class Light {
public:

	Light() :type(Point) {

	}

	Light(LightType type) {
		this->type = type;
	}

	float* AmbientPointer() {
		return &ambient.x;
	}

	float* DiffusePointer() {
		return &diffuse.x;
	}

	float* SpecularPointer() {
		return &specular.x;
	}

	glm::vec3 GetPosition() const {
		return position;
	}

	void SetPosition(glm::vec3 position) {
		this->position = position;
	}

	glm::vec3 GetAmbient() const {
		return ambient;
	}

	void SetAmbient(glm::vec3 ambient) {
		this->ambient = ambient;
	}

	glm::vec3 GetDiffuse() const {
		return diffuse;
	}

	void SetDiffuse(glm::vec3 diffuse) {
		this->diffuse = diffuse;
	}

	glm::vec3 GetSpecular() const {
		return specular;
	}

	void SetSpecular(glm::vec3 specular) {
		this->specular = specular;
	}

	void ResetAmbient() {
		ambient = AMBIENT_DEFAULT;
	}

	void ResetDiffuse() {
		diffuse = DIFFUSE_DEFAULT;
	}

	void ResetSpecular() {
		specular = SPECULAR_DEFAULT;
	}

	void ResetAll() {
		ResetAmbient();
		ResetDiffuse();
		ResetSpecular();
	}

private:

	inline static glm::vec3 AMBIENT_DEFAULT = glm::vec3(0.2f, 0.2f, 0.2f);
	inline static glm::vec3 DIFFUSE_DEFAULT = glm::vec3(1.0f, 1.0f, 1.0f);
	inline static glm::vec3 SPECULAR_DEFAULT = glm::vec3(1.0f, 1.0f, 1.0f);
	inline static glm::vec3 POSITION_DEFAULT = glm::vec3(0.0f, 0.0f, 0.0f);

	LightType type;

	glm::vec3 position = POSITION_DEFAULT;

	glm::vec3 ambient = AMBIENT_DEFAULT;
	glm::vec3 diffuse = DIFFUSE_DEFAULT;
	glm::vec3 specular = SPECULAR_DEFAULT;
};

#endif