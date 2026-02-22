#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "Model.h"

using namespace std;

class Entity {

public:
	Entity(Model model, Material material, int id, string name) {
		this->model = model;
		this->material = material;
		this->id = id;
		this->name = name;
		this->hasModel = true;
		this->hasMaterial = true;
	}

	Entity(int id, string name) {
		this->id = id;
		this->name = name;
		this->hasModel = false;
		this->hasMaterial = false;
	}

	int GetId() const {
		return id;
	}

	string GetName() const {
		return name;
	}

	void SetName(string name) {
		this->name = name;
	}

	bool HasModel() const {
		return hasModel;
	}

	bool HasMaterial() const {
		return hasMaterial;
	}

	Transform transform;
	Model model;
	Material material;

private:
	int id;
	string name;
	bool hasModel;
	bool hasMaterial;
};

#endif