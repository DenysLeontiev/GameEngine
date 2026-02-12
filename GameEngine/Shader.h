#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader {
public:
	unsigned int shaderProgramId;

	Shader(const char* vertexPath, const char* fragmentPath) {
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::istream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::istream::failbit | std::ifstream::badbit);

		try {
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);

			std::stringstream vShaderStream;
			std::stringstream fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

		} catch (const std::exception& err) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << err.what() << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertexShader;
		unsigned int fragmentShader;

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		checkCompileErrors(vertexShader, "VERTEX");

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		checkCompileErrors(fragmentShader, "FRAGMENT");

		shaderProgramId = glCreateProgram();

		glAttachShader(shaderProgramId, vertexShader);
		glAttachShader(shaderProgramId, fragmentShader);

		glLinkProgram(shaderProgramId);

		checkCompileErrors(shaderProgramId, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void useShaderProgram() {
		glUseProgram(shaderProgramId);
	}

	void setBool(const std::string& name, bool value) const
	{
		GLint location = getShaderUniformLocation(name);
		glUniform1i(location, (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		GLint location = getShaderUniformLocation(name);
		glUniform1i(location, value);
	}

	void setFloat(const std::string& name, float value) const
	{
		GLint location = getShaderUniformLocation(name);
		glUniform1f(location, value);
	}

	void setVec3(const std::string& name, glm::vec3 vector) const {
		GLint location = getShaderUniformLocation(name);
		glUniform3fv(location, 1, glm::value_ptr(vector));
	}

	void setVec4(const std::string& name, glm::vec4 vector) const {
		GLint location = getShaderUniformLocation(name);
		glUniform4fv(location, 1, glm::value_ptr(vector));
	}

	void setMat4(const std::string& name, glm::mat4 matrix) const {
		GLint location = getShaderUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	GLint getShaderUniformLocation(const std::string& name) const {
		const char* cString = name.c_str();
		GLint location = glGetUniformLocation(shaderProgramId, cString);

		return location;
	}

private:
	void checkCompileErrors(unsigned int shader, std::string type) {
		int success;
		char infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
			}
		}
	}
};

#endif
