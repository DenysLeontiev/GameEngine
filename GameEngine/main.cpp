#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<vector>
#include <iostream>

#include "stb_image.h"

#include "ApplicationUI.h"

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "Model.h"
#include "Hierarchy.h"

void createFramebuffer();
void rescaleFramebuffer(float width, float height);

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double currentX, double currentY);
void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

void renderScene(Hierarchy& hierarchy, Shader& modelShader, Shader& lightVisualShader,
	glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);

static float fpsTimer = 0.0f;
static int frameCount = 0;
float fps = 0.0f;

const GLint SCREEN_WIDTH = 800;
const GLint SCREEN_HEIGHT = 600;

GLuint FBO;
GLuint RBO;
GLuint textureId;

glm::vec3 cameraInitialPosition = glm::vec3(0.0f, 0.0f, 3.0f);
Camera camera(cameraInitialPosition);

float lastX = SCREEN_WIDTH / 2; 
float lastY = SCREEN_HEIGHT / 2;
bool isFirstCameraMove = true;
bool isRightMouseButtonHeld = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

ApplicationUI applicationUI;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

int main() {

	if (!glfwInit()) {
		std::cout << "GLFW failed to init" << "\n";
		glfwTerminate();
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game Engine", NULL, NULL);

	if (!mainWindow) {
		std::cout << "Failed to create window" << "\n";
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(mainWindow);

	glfwSetFramebufferSizeCallback(mainWindow, framebufferSizeCallback);
	glfwSetCursorPosCallback(mainWindow, mouseCallback);
	glfwSetScrollCallback(mainWindow, mouseScrollCallback);
	glfwSetMouseButtonCallback(mainWindow, mouseButtonCallback);

	glewExperimental = GL_TRUE;
	glEnable(GL_DEPTH_TEST);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to init glew" << "\n";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	Shader lightVisualShader(PathConsts::LIGHT_VERTEX_SHADER_PATH, PathConsts::LIGHT_FRAGMENT_SHADER_PATH);
	Shader modelShader(PathConsts::MODEL_VERTEX_SHADER_PATH, PathConsts::MODEL_FRAGMENT_SHADER_PATH);

	int bufferWidth = 0;
	int bufferHeight = 0;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glViewport(0, 0, bufferWidth, bufferHeight);

	createFramebuffer();

	applicationUI.Initialize(mainWindow);

	Hierarchy hierachy {};

	while (!glfwWindowShouldClose(mainWindow)) {

		processInput(mainWindow);

		glfwSetInputMode(mainWindow, GLFW_CURSOR, (isRightMouseButtonHeld ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		fpsTimer += deltaTime;
		frameCount++;

		if (fpsTimer > 1.0f) {
			fps = frameCount / fpsTimer;
			fpsTimer = 0.0f;
			frameCount = 0;
		}

		applicationUI.BeginFrame();

		 // Draw ImGui windows first to get viewport size
		applicationUI.DrawFramebuffer(textureId);

		applicationUI.DrawHierarchy(hierachy);
		applicationUI.DrawEditorWindow(hierachy);
		applicationUI.DrawTaskBar(fps, camera.zoom, isRightMouseButtonHeld);

		// Render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		applicationUI.UpdateViewportSize();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = camera.GetViewMatrix();
		applicationUI.ChangeProjectionMatrixDropdown(projection, camera.zoom, bufferWidth, bufferHeight);

		renderScene(hierachy, modelShader, lightVisualShader, view, projection, camera.cameraPosition);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		applicationUI.RenderImGuiViewports();
		applicationUI.EndFrame();

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	applicationUI.ShutdownImGui();

	glfwDestroyWindow(mainWindow);
	glfwTerminate();

	return 0;
}


void renderScene(Hierarchy& hierarchy, Shader& modelShader, Shader& lightVisualShader,
	glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition) {

	vector<Entity>& entities = hierarchy.GetEntities();

	std::vector<Light> directionalLights;
	std::vector<Light> pointLights;
	std::vector<Light> spotLights;

	for (Entity& e : entities) {
		if (!e.IsLight()) {
			continue;
		}

		e.UpdateLightProperties();

		if (e.light.GetLightType() == LightType::Directional) {
			directionalLights.push_back(e.light);
		}
		else if (e.light.GetLightType() == LightType::Point) {
			pointLights.push_back(e.light);
		}
		else if (e.light.GetLightType() == LightType::Spot) {
			spotLights.push_back(e.light);
		}
	}

	modelShader.useShaderProgram();
	modelShader.setMat4("view", view);
	modelShader.setMat4("projection", projection);
	modelShader.setVec3("viewPosition", cameraPosition);

	modelShader.setInt("numberOfDirectionalLights", directionalLights.size());
	modelShader.setInt("numberOfPointLights", pointLights.size());
	modelShader.setInt("numberOfSpotLights", spotLights.size());

	for (Entity& entity : entities) {
		if (entity.HasModel() && !entity.IsLight()) {
			if (entity.HasMaterial()) {
				glm::vec3 albedo = glm::vec3(entity.material.GetColor());
				modelShader.setVec3("material.ambient", 0.1f * albedo);
				modelShader.setVec3("material.diffuse", albedo);
				modelShader.setVec3("material.specular", glm::vec3(0.5f));
				modelShader.setFloat("material.shininess", 32.0f);
			} else {
				modelShader.setVec3("material.ambient", glm::vec3(0.1f));
				modelShader.setVec3("material.diffuse", glm::vec3(1.0f));
				modelShader.setVec3("material.specular", glm::vec3(0.5f));
				modelShader.setFloat("material.shininess", 32.0f);
			}

			glm::mat4 modelMatrix = entity.transform.GetModelMatrix();
			modelShader.setMat4("model", modelMatrix);

			entity.model.Draw(modelShader);
		}
	}

	for (int i = 0; i < pointLights.size(); ++i) {
		const auto& L = pointLights[i];
		std::string base = "pointLights[" + std::to_string(i) + "]";
		modelShader.setVec3(base + ".position", L.GetPosition());

		modelShader.setFloat(base + ".constant", L.GetConstant());
		modelShader.setFloat(base + ".linear", L.GetLinear());
		modelShader.setFloat(base + ".quadratic", L.GetQuadratic());

		modelShader.setVec3(base + ".ambient", L.GetAmbient());
		modelShader.setVec3(base + ".diffuse", L.GetDiffuse());
		modelShader.setVec3(base + ".specular", L.GetSpecular());
	}

	for (int i = 0; i < directionalLights.size(); ++i) {
		const auto& L = directionalLights[i];
		std::string base = "directionalLights[" + std::to_string(i) + "]";
		modelShader.setVec3(base + ".direction", L.GetDirection());

		modelShader.setVec3(base + ".ambient", L.GetAmbient());
		modelShader.setVec3(base + ".diffuse", L.GetDiffuse());
		modelShader.setVec3(base + ".specular", L.GetSpecular());
	}

	for (int i = 0; i < spotLights.size(); i++)
	{
		const auto& L = spotLights[i];
		std::string base = "spotLights[" + std::to_string(i) + "]";
		modelShader.setVec3(base + ".direction", L.GetDirection());
		modelShader.setVec3(base + ".position", L.GetPosition());

		modelShader.setFloat(base + ".cutOff", glm::cos(glm::radians(L.GetCutOffAngle())));
		modelShader.setFloat(base + ".outerCutOff", glm::cos(glm::radians(L.GetOuterCutOffAngle())));

		modelShader.setFloat(base + ".constant", L.GetConstant());
		modelShader.setFloat(base + ".linear", L.GetLinear());
		modelShader.setFloat(base + ".quadratic", L.GetQuadratic());

		modelShader.setVec3(base + ".ambient", L.GetAmbient());
		modelShader.setVec3(base + ".diffuse", L.GetDiffuse());
		modelShader.setVec3(base + ".specular", L.GetSpecular());
	}

	lightVisualShader.useShaderProgram();
	lightVisualShader.setMat4("view", view);
	lightVisualShader.setMat4("projection", projection);

	for (Entity& entity : hierarchy.GetEntities()) {
		if (entity.IsLight() && entity.HasModel()) {
			glm::mat4 modelMatrix = entity.transform.GetModelMatrix();
			lightVisualShader.setMat4("model", modelMatrix);

			entity.model.Draw(lightVisualShader);
		}
	}
}

void createFramebuffer() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void rescaleFramebuffer(float width, float height) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double currentX, double currentY) {

	if (isRightMouseButtonHeld == false) {
		return;
	}

	if (isFirstCameraMove == true) {
		lastX = currentX;
		lastY = currentY;
		isFirstCameraMove = false;
	}

	float xOffset = currentX - lastX;
	float yOffset = lastY - currentY;

	lastX = currentX;	
	lastY = currentY;

	camera.ProcessMouseInput(xOffset, yOffset);
}

void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {

	if (isRightMouseButtonHeld == false) {
		return;
	}

	float offset = static_cast<float>(yOffset);
	camera.ProcessMouseScroll(offset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			isRightMouseButtonHeld = true;
			isFirstCameraMove = true;
		}
		else if (action == GLFW_RELEASE) {
			isRightMouseButtonHeld = false;
			isFirstCameraMove = true;
		}
	}
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (isRightMouseButtonHeld) {

		float speedMultiplier = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			? MOVEMENT_SPEED_MULTIPLIER : 1.0f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(CameraMovement::FORWARD, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(CameraMovement::BACKWARD, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(CameraMovement::RIGHT, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(CameraMovement::LEFT, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(CameraMovement::UPWARD, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(CameraMovement::DOWNWARD, deltaTime, speedMultiplier);
		}
	}
}
