#include "ApplicationUI.h"

#include <iostream>

#include <limits>
#include <vector>
#include <filesystem>

#include "Model.h"

namespace fs = std::filesystem;

// External function declared in main.cpp
extern void rescaleFramebuffer(float width, float height);

ApplicationUI::ApplicationUI()
	: m_io(nullptr),
	m_viewportWidth(0.0f),
	m_viewportHeight(0.0f)
{
	uiProjectionMatrix.SetProjectionMatrixMode(ProjectionMatrixMode::Perspective);
	uiTheme.SetCurrentTheme(Theme::Dark);
	currentPath = fs::current_path();
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
	ImGui::Begin("Scene");

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

void ApplicationUI::DrawHierarchy(Hierarchy& hierarchy)
{
	ImGui::Begin("Hierarchy");

	auto& entities = hierarchy.GetEntities();
	Entity* selectedModel = hierarchy.GetSelectedEntity();

	for (size_t i = 0; i < entities.size(); i++)
	{
		Entity& entity = entities[i];

		bool isSelected = (selectedModel && entity.GetId() == selectedModel->GetId());

		if (ImGui::Selectable(entity.GetName().c_str(), isSelected))
			hierarchy.SetSelectedEntity(&entity);
	}

	DrawHierarchyTaskBar(hierarchy);

	ChangeThemeDropdown();
	ImGui::End();
}

void ApplicationUI::DrawHierarchyTaskBar(Hierarchy& hierarchy)
{
	Entity* selectedEntity = hierarchy.GetSelectedEntity();

	const float barHeight = 74.0f;

	ImGui::SetCursorPosY(ImGui::GetWindowHeight() - barHeight);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

	ImGui::BeginChild("##HierarchyFooter", ImVec2(0, barHeight), false,
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	const float w = ImGui::GetContentRegionAvail().x;

	if (ImGui::Button("Load", ImVec2(w, 0))) {
		ImGui::OpenPopup("LoadFilePopup");
	}

	static char renameBuffer[256] = "";
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted("Name:");
	ImGui::SameLine();

	const float btnW = 72.0f;
	const float inputW = w - btnW - ImGui::GetStyle().ItemSpacing.x - 42.0f;
	ImGui::SetNextItemWidth((inputW > 80.0f) ? inputW : 80.0f);

	bool canEdit = (selectedEntity != nullptr);

	if (!canEdit) {
		ImGui::BeginDisabled();
	}

	ImGui::InputText("##RenameModel", renameBuffer, sizeof(renameBuffer));
	ImGui::SameLine();

	bool doRename = ImGui::Button("Rename", ImVec2(btnW, 0));
	if (!canEdit) {
		ImGui::EndDisabled();
	}

	if (doRename && selectedEntity && renameBuffer[0] != '\0') {
		selectedEntity->SetName(renameBuffer);
		renameBuffer[0] = '\0';
	}

	if (selectedEntity) {
		if (ImGui::Button("Delete", ImVec2(w, 0)))
			hierarchy.RemoveEntity(selectedEntity->GetId());
	}
	else {
		ImGui::BeginDisabled();
		ImGui::Button("Delete", ImVec2(w, 0));
		ImGui::EndDisabled();
	}

	LoadFilePopup(hierarchy);

	ImGui::EndChild();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(3);
}

void ApplicationUI::DrawEditorWindow(Hierarchy& hierarchy) {
	Entity* selectedEntity = hierarchy.GetSelectedEntity();

	if (selectedEntity) {

		float stepOffset = 0.05f;
		ImGui::Begin("Settings");
		ImGui::DragFloat3("position (xyz)", selectedEntity->transform.PositionPointer(), stepOffset);
		ImGui::DragFloat3("rotation (xyz)", selectedEntity->transform.RotationPointer(), stepOffset);
		ImGui::DragFloat3("scale (xyz)", selectedEntity->transform.ScalePointer(), stepOffset);

		if (selectedEntity->HasMaterial()) {
			ImGui::ColorEdit4("color (rgba)", selectedEntity->material.ColorPointer(), stepOffset);
		}

		if (ImGui::Button("Reset Position")) {
			selectedEntity->transform.ResetPosition();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset Rotation")) {
			selectedEntity->transform.ResetRotation();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset Scale")) {
			selectedEntity->transform.ResetScale();
		}

		if (ImGui::Button("Reset All")) {
			selectedEntity->transform.ResetAll();
		}

		ImGui::End();
	}
}

void ApplicationUI::DrawTaskBar(float fps, float fov, bool isRMBHeld)
{
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove;

	ImGui::Begin("Task Bar", nullptr, flags);

	ImGui::Text("FPS: %.1f", fps);
	ImGui::SameLine(0, 20);

	ImGui::Text("FOV: %.1f", fov);
	ImGui::SameLine(0, 20);

	ImGui::Text("Camera Active: %s", isRMBHeld ? "True" : "False");

	ImGui::SameLine();

	ImGui::End();
}

void ApplicationUI::LoadFilePopup(Hierarchy& hierarchy) {
	if (ImGui::BeginPopup("LoadFilePopup")) {

		if (ImGui::Button("Up")) {
			currentPath = currentPath.parent_path();
		}

		ImGui::SeparatorText(currentPath.string().c_str());

		for (const auto& entry : fs::directory_iterator(currentPath)) {

			const bool isDirectory = entry.is_directory();
			const bool isRegularFile = entry.is_regular_file();

			std::string entryName = entry.path().filename().string();
			std::string displayEntryName = isDirectory
				? "[D] " + entryName : isRegularFile
				? "[F] " + entryName : "";

			ImGuiSelectableFlags flags = ImGuiSelectableFlags_DontClosePopups;


			if (ImGui::Selectable(displayEntryName.c_str(), false, flags)) {
				if (isDirectory) {
					currentPath /= entry.path().filename();
				}

				if (isRegularFile) {
					int modelId = hierarchy.GetNextId();
					string modelName = entryName + "(" + to_string(modelId) + ")";
					string modelPath = entry.path().string();

					Model newModel;
					bool isLoaded = newModel.AttachModel(modelPath);

					if (isLoaded) {
						Material entityMaterial;
						Entity entity(newModel, entityMaterial, modelId, modelName);
						hierarchy.AddEntity(entity);
					}

					ImGui::EndPopup();
					return;
				}
			}
		}

		if (ImGui::MenuItem("Close")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ApplicationUI::ChangeThemeDropdown() {
	
	ImGui::Begin("Editor");

	std::string* themes = uiTheme.GetAvailableThemes();
	Theme currentTheme = uiTheme.GetCurrentTheme();

	if (ImGui::BeginCombo("##Theme", UITheme::ThemeToString(currentTheme).c_str())) {
		for (size_t i = 0; i < THEME_COUNT; i++) {
			bool isSelected = UITheme::ThemeToString(currentTheme) == themes[i];

			if (ImGui::Selectable((themes[i]).c_str(), isSelected)) {
				currentTheme = UITheme::StringToTheme(themes[i]);
				uiTheme.SetCurrentTheme(currentTheme);

				switch (currentTheme) {
					case Theme::Dark:
						ImGui::StyleColorsDark();
						break;
					case Theme::Light:
						ImGui::StyleColorsLight();
						break;
					case Theme::Classic:
						ImGui::StyleColorsClassic();
						break;
				}
			}
		}

		ImGui::EndCombo();
	}

	ImGui::End();
}

void ApplicationUI::ChangeProjectionMatrixDropdown(glm::mat4& projectionMatrix,float cameraZoom, int bufferWidth, int bufferHeight) {

	ImGui::Begin("Projection");

	std::string* modes = uiProjectionMatrix.GetAvailableMatrixModes();
	ProjectionMatrixMode currentMode = uiProjectionMatrix.GetCurrentProjectionMatrixMode();

	if (ImGui::BeginCombo("##ProjectionMode",
		UIProjectionMatrix::ProjectionModeToString(currentMode).c_str()))
	{
		for (int i = 0; i < PROJECTION_MATRIX_COUNT; i++)
		{
			bool isSelected = (UIProjectionMatrix::ProjectionModeToString(currentMode) == modes[i]);

			if (ImGui::Selectable(modes[i].c_str(), isSelected))
			{
				currentMode = UIProjectionMatrix::StringToProjectionMode(modes[i]);
				uiProjectionMatrix.SetProjectionMatrixMode(currentMode);
			}
		}

		ImGui::EndCombo();
	}

	currentMode = uiProjectionMatrix.GetCurrentProjectionMatrixMode();

	if (currentMode == ProjectionMatrixMode::Perspective)
	{
		projectionMatrix = glm::perspective(
			glm::radians(cameraZoom),
			(float)bufferWidth / (float)bufferHeight,
			0.1f,
			100.0f
		);
	}
	else if(currentMode == ProjectionMatrixMode::Orthogonal)
	{
		float aspectRatio = (float)bufferWidth / (float)bufferHeight;
		float orthoSize = 10.0f;

		projectionMatrix = glm::ortho(
			-orthoSize * aspectRatio, orthoSize * aspectRatio,
			-orthoSize, orthoSize,
			0.1f, 100.0f
		);
	}

	ImGui::End();
}

void ApplicationUI::ShutdownImGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
