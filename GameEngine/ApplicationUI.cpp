#include "ApplicationUI.h"

// External function declared in main.cpp
extern void rescaleFramebuffer(float width, float height);

ApplicationUI::ApplicationUI()
	: m_io(nullptr),
	m_viewportWidth(0.0f),
	m_viewportHeight(0.0f)
{

}

void ApplicationUI::Initialize(GLFWwindow* mainWindow) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	m_io = &ImGui::GetIO(); (void)*m_io;
	
	// Enable docking - this must be done during initialization, BEFORE the backends
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
	const char* openGlVersion = "#version 330";
	ImGui_ImplOpenGL3_Init(openGlVersion);
}

void ApplicationUI::SetupDockspace() {
	// Create a fullscreen dockspace - call this every frame!
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	// Create the dockspace
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
}

void ApplicationUI::BeginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Setup dockspace every frame
	SetupDockspace();
}

void ApplicationUI::EndFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ApplicationUI::DrawFramebuffer(GLint textureId) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Editor");

	// Get available size in this window
	ImVec2 size = ImGui::GetContentRegionAvail();

	// Only rescale if size actually changed
	if (size.x != m_viewportWidth || size.y != m_viewportHeight) {
		m_viewportWidth = size.x;
		m_viewportHeight = size.y;
		if (m_viewportWidth > 0 && m_viewportHeight > 0) {
			rescaleFramebuffer(m_viewportWidth, m_viewportHeight);
		}
	}

	// Display the framebuffer texture, flipped vertically
	ImGui::Image(
		(ImTextureID)(intptr_t)textureId,
		size,
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	ImGui::End();
	ImGui::PopStyleVar();
}

void ApplicationUI::UpdateViewportSize() {
	// Just set the viewport for rendering
	glViewport(0, 0, static_cast<GLsizei>(m_viewportWidth), static_cast<GLsizei>(m_viewportHeight));
}

void ApplicationUI::RenderImGuiViewports() {
	if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void ApplicationUI::DrawEditorWindow(float color[]) {
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Cube Color", color);
	ImGui::End();
}

void ApplicationUI::ShutdownUpImGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
