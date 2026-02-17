#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <vector>
#include "Model.h"

class Hierarchy {
public:
	std::vector<Model>& GetModels() {
		return models;
	}

	const std::vector<Model>& GetModels() const {
		return models;
	}

	void AddModel(const Model& model) {
		models.push_back(model);
	}

	void RemoveModel(int id) {
		if (selectedModel && selectedModel->id == id)
			selectedModel = nullptr;

		models.erase(
			std::remove_if(models.begin(), models.end(),
				[id](const Model& m) { return m.id == id; }),
			models.end()
		);
	}

	Model* GetSelectedModel() const {
		return selectedModel;
	}

	void SetSelectedModel(Model* model) {
		selectedModel = model;
	}

	int GetNextId() const {
		int currentSize = models.size();
		currentSize++;
		return currentSize;
	}

private:
	std::vector<Model> models;

	Model* selectedModel = nullptr;
};

#endif 