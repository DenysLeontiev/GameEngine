#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>


void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const GLint SCREEN_WIDTH = 800;
const GLint SCREEN_HEIGHT = 600;

GLuint VAO;
GLuint VBO;
GLuint FBO;
GLuint RBO;
GLuint textureId;
GLuint shaderProgramId;

const char* vertexShader = R"glsl(
#version 330

layout (location = 0) in vec3 aPos;

void main() {
	gl_Position = vec4(aPos, 1.0f);
}
)glsl";

const char* fragmentShader = R"glsl(
#version 330

out vec4 aColor;

void main() {
	aColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
)glsl";

void createTriangle();
void addShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType);
void createAndCompileShader();

void createFramebuffer();
void rescaleFramebuffer(float width, float height);

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

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to init glew" << "\n";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	int bufferWidth = 0;
	int bufferHeight = 0;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glViewport(0, 0, bufferWidth, bufferHeight);

	createTriangle();
	createAndCompileShader();
	createFramebuffer();

	////Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
	const char* openGlVersion = "#version 330";
	ImGui_ImplOpenGL3_Init(openGlVersion);

	while (!glfwWindowShouldClose(mainWindow)) {
		processInput(mainWindow);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui Window
		ImGui::Begin("Game Engine");

		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float windowHeight = ImGui::GetContentRegionAvail().y;

		// Render to framebuffer FIRST (before using the texture in ImGui)
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		rescaleFramebuffer(windowWidth, windowHeight);
		glViewport(0, 0, static_cast<GLsizei>(windowWidth), static_cast<GLsizei>(windowHeight));

		// Clear the framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw triangle to framebuffer
		glUseProgram(shaderProgramId);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Now display the framebuffer texture in ImGui
		ImVec2 pos = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddImage(
			(void*)(intptr_t)textureId,
			ImVec2(pos.x, pos.y),
			ImVec2(pos.x + windowWidth, pos.y + windowHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::End();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(mainWindow);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Clean up
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

	return 0;
}

void createTriangle() {
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // 3 * sizeof(GL_FLOAT)
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void addShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType) {
	GLuint shaderToCreate = glCreateShader(shaderType);

	const GLchar* code[1];
	code[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(shaderToCreate, 1, code, codeLength);
	glCompileShader(shaderToCreate);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(shaderToCreate, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(shaderToCreate, sizeof(eLog), NULL, eLog);
		std::cout << "Error compiling " << shaderType << " shader: " << eLog << "\n";
		return;
	}

	glAttachShader(shaderProgram, shaderToCreate);
}

void createAndCompileShader() {
	shaderProgramId = glCreateProgram();

	if (!shaderProgramId) {
		std::cout << "Error creating shader program!\n";
		exit(1);
	}

	addShader(shaderProgramId, vertexShader, GL_VERTEX_SHADER);
	addShader(shaderProgramId, fragmentShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderProgramId);
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderProgramId, sizeof(eLog), NULL, eLog);
		std::cout << "Error linking program:\n" << eLog << '\n';
		return;
	}

	glValidateProgram(shaderProgramId);
	glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderProgramId, sizeof(eLog), NULL, eLog);
		std::cout << "Error validating program:\n" << eLog << '\n';
		return;
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

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}