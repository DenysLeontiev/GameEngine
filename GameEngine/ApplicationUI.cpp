#include "ApplicationUI.h"

#include <iostream>

#include <limits>
#include <vector>
#include <filesystem>

#include "Model.h"
#include "PathConsts.h"
#include "UIConsts.h"

namespace fs = std::filesystem;

// External function declared in main.cpp
extern void rescaleFramebuffer(float width, float height);

ApplicationUI::ApplicationUI()
	: m_io(nullptr),
	m_viewportWidth(0.0f),
	m_viewportHeight(0.0f)
{
	uiProjectionMatrix.SetProjectionMatrixMode(ProjectionMatrixMode::Perspective);
	uiTheme.SetCurrentTheme(Theme::Modern);

	if (defaultPath.empty()) {
		currentPath = fs::current_path();
	}
	else {
		currentPath = defaultPath;
	}
}

void ApplicationUI::Initialize(GLFWwindow* mainWindow) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	m_io = &ImGui::GetIO(); (void)*m_io;
	
	// Enable docking - this must be done during initialization, BEFORE the backends
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	UITheme::ApplyModernStyle();
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

void ApplicationUI::UpdateViewportSize() const {
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

	ImGui::TextDisabled("ENTITIES");
	ImGui::Separator();
	ImGui::Spacing();

	float listHeight = ImGui::GetContentRegionAvail().y - UIConsts::HIERARCHY_FOOTER_HEIGHT;
	if (listHeight < UIConsts::HIERARCHY_MIN_LIST_HEIGHT) listHeight = UIConsts::HIERARCHY_MIN_LIST_HEIGHT;

	ImGui::BeginChild("##EntityList", ImVec2(0, listHeight), false);
	for (size_t i = 0; i < entities.size(); i++)
	{
		Entity& entity = entities[i];
		bool isSelected = (selectedModel && entity.GetId() == selectedModel->GetId());

		std::string label;
		if (entity.IsLight()) {
			label = "* " + entity.GetName();
		}
		else {
			label = "  " + entity.GetName();
		}

		if (ImGui::Selectable(label.c_str(), isSelected)) {
			hierarchy.SetSelectedEntity(&entity);
		}
	}
	ImGui::EndChild();

	ImGui::Spacing();
	DrawHierarchyTaskBar(hierarchy);

	ChangeThemeDropdown();
	ImGui::End();
}

void ApplicationUI::DrawHierarchyTaskBar(Hierarchy& hierarchy)
{
	Entity* selectedEntity = hierarchy.GetSelectedEntity();

	const float w = ImGui::GetContentRegionAvail().x;

	ImGui::PushStyleColor(ImGuiCol_Button, UIConsts::ACCENT_BUTTON);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UIConsts::ACCENT_BUTTON_HOVERED);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, UIConsts::ACCENT_BUTTON_ACTIVE);
	if (ImGui::Button("Load Model", ImVec2(w, 0))) {
		ImGui::OpenPopup("LoadFilePopup");
	}
	ImGui::PopStyleColor(3);

	ImGui::Spacing();

	ImGui::TextDisabled("ADD LIGHTS");
	ImGui::Separator();
	ImGui::Spacing();

	float halfW = (w - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

	if (ImGui::Button("Directional", ImVec2(halfW, 0))) {
		Entity directionalLightEntity = Entity::CreateDirectionalLight("Directional Light", PathConsts::DIRECTIONAL_LIGHT_VISUAL_PATH);
		hierarchy.AddEntity(directionalLightEntity);
	}
	ImGui::SameLine();
	if (ImGui::Button("Point", ImVec2(halfW, 0))) {
		Entity pointLightEntity = Entity::CreatePointLightEntity("Point Light", PathConsts::POINT_LIGHT_VISUAL_PATH);
		hierarchy.AddEntity(pointLightEntity);
	}

	if (ImGui::Button("Spot", ImVec2(w, 0))) {
		Entity spotLightEntity = Entity::CreateSpotLightEntity("Spot Light", PathConsts::SPOT_LIGHT_VISUAL_PATH);
		hierarchy.AddEntity(spotLightEntity);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static char renameBuffer[256] = "";
	bool canEdit = (selectedEntity != nullptr);

	if (!canEdit) {
		ImGui::BeginDisabled();
	}

	const float btnW = UIConsts::RENAME_BUTTON_WIDTH;
	const float inputW = w - btnW - ImGui::GetStyle().ItemSpacing.x;
	ImGui::SetNextItemWidth((inputW > UIConsts::RENAME_INPUT_MIN_WIDTH) ? inputW : UIConsts::RENAME_INPUT_MIN_WIDTH);
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

	ImGui::Spacing();
	if (selectedEntity) {
		ImGui::PushStyleColor(ImGuiCol_Button, UIConsts::DANGER_BUTTON);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UIConsts::DANGER_BUTTON_HOVERED);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UIConsts::DANGER_BUTTON_ACTIVE);
		if (ImGui::Button("Delete", ImVec2(w, 0))) {
			hierarchy.RemoveEntity(selectedEntity->GetId());
		}
		ImGui::PopStyleColor(3);
	}
	else {
		ImGui::BeginDisabled();
		ImGui::Button("Delete", ImVec2(w, 0));
		ImGui::EndDisabled();
	}

	LoadFilePopup(hierarchy);
}

void ApplicationUI::DrawEditorWindow(Hierarchy& hierarchy) {
	Entity* selectedEntity = hierarchy.GetSelectedEntity();

	ImGui::Begin("Settings");

	if (!selectedEntity) {
		ImGui::Spacing();
		ImGui::TextDisabled("Select an entity to edit its properties.");
		ImGui::End();
		return;
	}

	ImGui::TextDisabled("EDITING:");
	ImGui::SameLine();
	ImGui::Text("%s", selectedEntity->GetName().c_str());
	ImGui::Separator();
	ImGui::Spacing();

	float stepOffset = UIConsts::TRANSFORM_STEP;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Indent(UIConsts::SECTION_INDENT);
		ImGui::Spacing();

		ImGui::DragFloat3("Position", selectedEntity->transform.PositionPointer(), stepOffset);
		ImGui::DragFloat3("Rotation", selectedEntity->transform.RotationPointer(), stepOffset);
		ImGui::DragFloat3("Scale", selectedEntity->transform.ScalePointer(), stepOffset);

		ImGui::Spacing();

		float halfW = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
		if (ImGui::Button("Reset Position", ImVec2(halfW, 0))) {
			selectedEntity->transform.ResetPosition();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Rotation", ImVec2(halfW, 0))) {
			selectedEntity->transform.ResetRotation();
		}

		if (ImGui::Button("Reset Scale", ImVec2(halfW, 0))) {

			selectedEntity->transform.ResetScale();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset All##Transform", ImVec2(halfW, 0))) {

			selectedEntity->transform.ResetAll();
		}

		ImGui::Spacing();
		ImGui::Unindent(UIConsts::SECTION_INDENT);
	}

	if (selectedEntity->HasMaterial()) {
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent(UIConsts::SECTION_INDENT);
			ImGui::Spacing();

			ImGui::ColorEdit4("Color", selectedEntity->material.ColorPointer());

			ImGui::Spacing();
			ImGui::Unindent(UIConsts::SECTION_INDENT);
		}
	}

	if (selectedEntity->HasLight()) {
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent(UIConsts::SECTION_INDENT);
			ImGui::Spacing();

			Light& selectedLight = selectedEntity->light;

			ImGui::DragFloat3("Ambient", selectedLight.AmbientPointer(), UIConsts::LIGHT_COLOR_STEP, 0.0f, 1.0f);
			ImGui::DragFloat3("Diffuse", selectedLight.DiffusePointer(), UIConsts::LIGHT_COLOR_STEP, 0.0f, 1.0f);
			ImGui::DragFloat3("Specular", selectedLight.SpecularPointer(), UIConsts::LIGHT_COLOR_STEP, 0.0f, 1.0f);

			if (selectedLight.GetLightType() == LightType::Point || selectedLight.GetLightType() == LightType::Spot) {
				ImGui::Spacing();
				ImGui::TextDisabled("ATTENUATION");
				ImGui::DragFloat("Constant", selectedLight.ConstantPointer(), UIConsts::ATTENUATION_CONSTANT_STEP, 0.0f, 10.0f);
				ImGui::DragFloat("Linear", selectedLight.LinearPointer(), UIConsts::ATTENUATION_LINEAR_STEP, 0.0f, 1.0f);
				ImGui::DragFloat("Quadratic", selectedLight.QuadraticPointer(), UIConsts::ATTENUATION_QUAD_STEP, 0.0f, 1.0f);
			}

			if (selectedLight.GetLightType() == LightType::Spot) {
				ImGui::Spacing();
				ImGui::TextDisabled("CONE");
				ImGui::DragFloat("Cut Off", selectedLight.CutOffAnglePointer(), UIConsts::CONE_ANGLE_STEP, 0.0f, 360.0f);
				ImGui::DragFloat("Outer Cut Off", selectedLight.OuterCutOffAnglePointer(), UIConsts::CONE_ANGLE_STEP, 0.0f, 360.0f);
			}

			ImGui::Spacing();

			float halfW = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

			if (ImGui::Button("Reset Ambient", ImVec2(halfW, 0))) {
				selectedLight.ResetAmbient();
			}

			ImGui::SameLine();

			if (ImGui::Button("Reset Diffuse", ImVec2(halfW, 0))) {
				selectedLight.ResetDiffuse();
			}

			if (ImGui::Button("Reset Specular", ImVec2(halfW, 0))) {
				selectedLight.ResetSpecular();
			}

			ImGui::SameLine();

			if (ImGui::Button("Reset All##Light", ImVec2(halfW, 0))) {
				selectedLight.ResetAll();
			}

			ImGui::Spacing();
			ImGui::Unindent(UIConsts::SECTION_INDENT);
		}
	}

	ImGui::End();
}

void ApplicationUI::DrawTaskBar(float fps, float fov, bool isRMBHeld)
{
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove;

	ImGui::Begin("Task Bar", nullptr, flags);

	if (fps >= UIConsts::FPS_GOOD_THRESHOLD) {
		ImGui::TextColored(UIConsts::STATUS_GOOD, "FPS: %.0f", fps);
	}
	else if (fps >= UIConsts::FPS_OK_THRESHOLD) {
		ImGui::TextColored(UIConsts::STATUS_WARNING, "FPS: %.0f", fps);
	}
	else {
		ImGui::TextColored(UIConsts::STATUS_BAD, "FPS: %.0f", fps);
	}

	ImGui::SameLine(0, UIConsts::TASKBAR_SPACING);

	ImGui::TextDisabled("|");
	ImGui::SameLine(0, UIConsts::TASKBAR_SPACING);

	ImGui::Text("FOV: %.1f", fov);
	ImGui::SameLine(0, UIConsts::TASKBAR_SPACING);

	ImGui::TextDisabled("|");
	ImGui::SameLine(0, UIConsts::TASKBAR_SPACING);

	if (isRMBHeld) {
		ImGui::TextColored(UIConsts::STATUS_GOOD, "Camera Active");
	}
	else {
		ImGui::TextDisabled("Camera Inactive");
	}

	ImGui::End();
}

void ApplicationUI::LoadFilePopup(Hierarchy& hierarchy) {
	if (ImGui::BeginPopup("LoadFilePopup")) {

		ImGui::PushStyleColor(ImGuiCol_Button, UIConsts::SUBTLE_BUTTON);
		if (ImGui::Button("<  Up", ImVec2(UIConsts::FILE_POPUP_UP_BTN_WIDTH, 0))) {
			currentPath = currentPath.parent_path();
		}
		ImGui::PopStyleColor();

		ImGui::Spacing();
		ImGui::TextDisabled("%s", currentPath.string().c_str());
		ImGui::Separator();
		ImGui::Spacing();

		for (const auto& entry : fs::directory_iterator(currentPath)) {

			const bool isDirectory = entry.is_directory();
			const bool isRegularFile = entry.is_regular_file();

			std::string entryName = entry.path().filename().string();
			std::string displayEntryName = isDirectory
				? "> " + entryName : isRegularFile
				? "  " + entryName : "";

			ImGuiSelectableFlags flags = ImGuiSelectableFlags_DontClosePopups;


			if (ImGui::Selectable(displayEntryName.c_str(), false, flags)) {
				if (isDirectory) {
					currentPath /= entry.path().filename();
				}

				if (isRegularFile) {
					string modelPath = entry.path().string();

					Model newModel;
					bool isLoaded = newModel.AttachModel(modelPath);

					if (isLoaded) {
						Material entityMaterial;
						Entity entity(newModel, entityMaterial, entryName);
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

	ImGui::TextDisabled("APPEARANCE");
	ImGui::Spacing();

	std::string* themes = uiTheme.GetAvailableThemes();
	Theme currentTheme = uiTheme.GetCurrentTheme();

	if (ImGui::BeginCombo("##Theme", UITheme::ThemeToString(currentTheme).c_str())) {
		for (size_t i = 0; i < static_cast<int>(Theme::THEME_COUNT); i++) {
			bool isSelected = UITheme::ThemeToString(currentTheme) == themes[i];

			if (ImGui::Selectable((themes[i]).c_str(), isSelected)) {
				currentTheme = UITheme::StringToTheme(themes[i]);
				uiTheme.SetCurrentTheme(currentTheme);

				switch (currentTheme) {
					case Theme::Modern:
						UITheme::ApplyModernStyle();
						break;
					case Theme::DarkMode:
						ImGui::StyleColorsDark();
						UITheme::ApplyModernShaping();
						break;
					case Theme::LightMode:
						ImGui::StyleColorsLight();
						UITheme::ApplyModernShaping();
						break;
					case Theme::ClassicMode:
						ImGui::StyleColorsClassic();
						UITheme::ApplyModernShaping();
						break;
				}
			}
		}

		ImGui::EndCombo();
	}

	ImGui::End();
}

void ApplicationUI::ChangeProjectionMatrixDropdown(glm::mat4& projectionMatrix, 
	float cameraZoom, int bufferWidth, int bufferHeight, bool& applyLights) {

	ImGui::Begin("Projection");

	ImGui::TextDisabled("CAMERA");
	ImGui::Spacing();

	std::string* modes = uiProjectionMatrix.GetAvailableMatrixModes();
	ProjectionMatrixMode currentMode = uiProjectionMatrix.GetCurrentProjectionMatrixMode();

	if (ImGui::BeginCombo("##ProjectionMode",
		UIProjectionMatrix::ProjectionModeToString(currentMode).c_str()))
	{
		for (int i = 0; i < static_cast<int>(ProjectionMatrixMode::PROJECTION_MATRIX_COUNT); i++)
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

	if (ImGui::Checkbox("Apply Lights", &applyLights)) { }

	ImGui::End();
}

void ApplicationUI::ShutdownImGui() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
