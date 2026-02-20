#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <filesystem>

#include "Hierarchy.h"
#include "UITheme.h"
#include "UIProjectionMatrix.h"

namespace fs = std::filesystem;

class ApplicationUI
{
public:
	ApplicationUI();

	void Initialize(GLFWwindow* mainWindow);
	void BeginFrame();
	void EndFrame();
	void DrawFramebuffer(GLint textureId);
	void UpdateViewportSize();
	void SetupDockspace();
	void RenderImGuiViewports();
	void DrawHierarchy(Hierarchy& hierarchy);
	void DrawHierarchyTaskBar(Hierarchy& hierarchy);
	void DrawEditorWindow(Hierarchy& hierarchy);
	void LoadFilePopup(Hierarchy& hierarchy);
	void ChangeThemeDropdown();
	void ChangeProjectionMatrixDropdown(glm::mat4& projectionMatrix, float cameraZoom, int bufferWidth, int bufferHeight);
	void DrawTaskBar(float fps, float fov, bool isRMBHeld);
	void ShutdownImGui();
private:
	ImGuiIO* m_io;
	float m_viewportWidth;
	float m_viewportHeight;

	UITheme uiTheme;
	UIProjectionMatrix uiProjectionMatrix;

	fs::path currentPath {};
};

