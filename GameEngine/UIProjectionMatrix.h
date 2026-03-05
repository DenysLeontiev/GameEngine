#ifndef UI_PROJECTION_MATRIX_H
#define UI_PROJECTION_MATRIX_H

#include <string>

enum class ProjectionMatrixMode {
	Perspective = 0,
	Orthogonal = 1,

	PROJECTION_MATRIX_COUNT = 2
};

struct UIProjectionMatrix
{
public:

	UIProjectionMatrix() {
		currentProjectionMatrixMode = ProjectionMatrixMode::Perspective;
	}

	UIProjectionMatrix(ProjectionMatrixMode projectionMatrixMode) {
		currentProjectionMatrixMode = projectionMatrixMode;
	}

	std::string* GetAvailableMatrixModes() {
		return availableMatrices;
	}

	ProjectionMatrixMode GetCurrentProjectionMatrixMode() {
		return currentProjectionMatrixMode;
	}

	void SetProjectionMatrixMode(ProjectionMatrixMode mode) {
		this->currentProjectionMatrixMode = mode;
	}

	static std::string ProjectionModeToString(ProjectionMatrixMode projectionMatrix) {
		switch (projectionMatrix) {
		case ProjectionMatrixMode::Perspective:
			return "Perspective";
		case ProjectionMatrixMode::Orthogonal:
			return "Orthogonal";
		default:
			return "Perspective";
		}
	}

	static ProjectionMatrixMode StringToProjectionMode(std::string projection) {
		if (projection == ProjectionModeToString(ProjectionMatrixMode::Orthogonal)) {
			return ProjectionMatrixMode::Orthogonal;
		} else if (projection == ProjectionModeToString(ProjectionMatrixMode::Perspective)) {
			return ProjectionMatrixMode::Perspective;
		}

		return ProjectionMatrixMode::Perspective;
	}

private:
	std::string availableMatrices[static_cast<int>(ProjectionMatrixMode::PROJECTION_MATRIX_COUNT)] = {
		ProjectionModeToString(ProjectionMatrixMode::Perspective), 
		ProjectionModeToString(ProjectionMatrixMode::Orthogonal) 
	};

	ProjectionMatrixMode currentProjectionMatrixMode;
};

#endif