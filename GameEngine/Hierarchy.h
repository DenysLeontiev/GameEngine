#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <vector>
#include "Model.h"
#include "Entity.h"

class Hierarchy {
public:
	std::vector<Entity>& GetEntities() {
		return entities;
	}

	const std::vector<Entity>& GetEntities() const {
		return entities;
	}

	void AddEntity(Entity& entity) {

		int id = objectNumerationTracker++;
		entity.SetId(id);

		string currentName = entity.GetName();
		string modelName = currentName + "(" + to_string(id) + ")";
		entity.SetName(modelName);

		selectedEntity = nullptr; // reset selection when we add new entity

		entities.push_back(entity);
	}

	void RemoveEntity(int id) {
		if (selectedEntity && selectedEntity->GetId() == id) {
			selectedEntity = nullptr;
		}

		entities.erase(
			std::remove_if(entities.begin(), entities.end(),
				[id](const Entity& m) { return m.GetId() == id; }),
			entities.end()
		);
	}

	Entity* GetSelectedEntity() const {
		return selectedEntity;
	}

	void SetSelectedEntity(Entity* entity) {
		selectedEntity = entity;
	}

private:
	std::vector<Entity> entities;

	Entity* selectedEntity = nullptr;

	unsigned int objectNumerationTracker { 0 };
};

#endif 