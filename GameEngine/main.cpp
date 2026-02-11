#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<vector>
#include <iostream>

#include "ApplicationUI.h"

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

void createFramebuffer();
void rescaleFramebuffer(float width, float height);

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double currentX, double currentY);
void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

static float fpsTimer = 0.0f;
static int frameCount = 0;
float fps = 0.0f;

const GLint SCREEN_WIDTH = 800;
const GLint SCREEN_HEIGHT = 600;

GLuint VAO;
GLuint VBO;
GLuint FBO;
GLuint RBO;
GLuint textureId;

glm::vec3 cameraInitialPosition = glm::vec3(0.0f, 0.0f, 3.0f);
Camera camera(cameraInitialPosition);

float pitch = 0.0f;
float yaw = -90.0f;
float lastX = SCREEN_WIDTH / 2; 
float lastY = SCREEN_HEIGHT / 2;
bool isFirstCameraMove = true;

bool isRightMouseButtonHeld = false;


float deltaTime = 0.0f;
float lastFrame = 0.0f;


std::vector<Vertex> cubeVertices = {
	// FRONT (+Z)
	{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	{{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

	// BACK (-Z)
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1.0f, 0.0f}},
	{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {1.0f, 1.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {0.0f, 1.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f,-1.0f}, {0.0f, 0.0f}},

	// LEFT (-X)
	{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
	{{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},

	// RIGHT (+X)
	{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
	{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

	// TOP (+Y)
	{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
	{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},

	// BOTTOM (-Y)
	{{-0.5f, -0.5f, -0.5f}, {0.0f,-1.0f, 0.0f}, {1.0f, 1.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {0.0f,-1.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {0.0f,-1.0f, 0.0f}, {0.0f, 0.0f}},
	{{-0.5f, -0.5f,  0.5f}, {0.0f,-1.0f, 0.0f}, {1.0f, 0.0f}},
};

vector<unsigned int> cubeIndices = {
	0, 1, 2,  2, 3, 0,        // front
	4, 5, 6,  6, 7, 4,        // back
	8, 9,10, 10,11, 8,        // left
   12,13,14, 14,15,12,        // right
   16,17,18, 18,19,16,        // top
   20,21,22, 22,23,20         // bottom
};

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

	Shader shader("assets/shaders/basic/basic.vert", "assets/shaders/basic/basic.frag");

	int bufferWidth = 0;
	int bufferHeight = 0;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glViewport(0, 0, bufferWidth, bufferHeight);

	//createCube();
	createFramebuffer();

	ApplicationUI applicationUI;
	applicationUI.Initialize(mainWindow);

	Mesh mesh(cubeVertices, cubeIndices);

	shader.useShaderProgram();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setMat4("model", model);

	glm::mat4 view = glm::mat4(1.0f);
	shader.setMat4("view", view);

	glm::mat4 projection = glm::mat4(1.0f);
	shader.setMat4("projection", projection);

	float cubeColor[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	float positionVec3f[3] = { 0.0f, 0.0f, 0.0f };
	float rotationVec3f[3] = { 0.0f, 0.0f, 0.0f };
	float scaleVec3f[3] = { 1.0f, 1.0f, 1.0f };

	while (!glfwWindowShouldClose(mainWindow)) {

		processInput(mainWindow);

		glfwSetInputMode(mainWindow, GLFW_CURSOR, (isRightMouseButtonHeld ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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
		applicationUI.DrawEditorWindow(cubeColor, positionVec3f, rotationVec3f, scaleVec3f);
		applicationUI.DrawTaskBar(fps, camera.zoom, isRightMouseButtonHeld);

		// Render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		applicationUI.UpdateViewportSize();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.useShaderProgram();

		mesh.draw();

		shader.setVec4("ourColor", glm::vec4(cubeColor[0], cubeColor[1], cubeColor[2], cubeColor[3]));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(positionVec3f[0], positionVec3f[1], positionVec3f[2]));
		model = glm::rotate(model, glm::radians(rotationVec3f[0]), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotationVec3f[1]), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotationVec3f[2]), glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(scaleVec3f[0], scaleVec3f[1], scaleVec3f[2]));
		shader.setMat4("model", model);

		view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		projection = glm::perspective(glm::radians(camera.zoom), (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);

		shader.setMat4("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		applicationUI.RenderImGuiViewports();
		applicationUI.EndFrame();

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	applicationUI.ShutdownUpImGui();

	// Clean up
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

	return 0;
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
			camera.ProcessKeyboardInput(FORWARD, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(BACKWARD, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(RIGHT, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(LEFT, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(UPWARD, deltaTime, speedMultiplier);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			camera.ProcessKeyboardInput(DOWNWARD, deltaTime, speedMultiplier);
		}
	}
}