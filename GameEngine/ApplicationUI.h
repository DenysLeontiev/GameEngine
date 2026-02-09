#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
	void DrawEditorWindow(float color[4], float positionVec3f[3], float rotationVec3f[3], float scaleVec3f[3]);
	void DrawTaskBar(float fps, float fov, bool isRMBHeld);
	void ShutdownUpImGui();
private:
	ImGuiIO* m_io;
	float m_viewportWidth;
	float m_viewportHeight;
};

