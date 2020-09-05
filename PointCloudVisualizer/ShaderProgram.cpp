#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>

#include "glm/gtc/type_ptr.hpp"


ShaderProgram::ShaderProgram() : mHandle(0) {

}


ShaderProgram::~ShaderProgram() {
	glDeleteProgram(mHandle);
}


bool ShaderProgram::loaderShaders(const char* vsFilename, const char* fsFilename) {
	std::string vsString = fileToString(vsFilename);
	std::string fsString = fileToString(fsFilename);
	const GLchar* vsSourcePtr = vsString.c_str();
	const GLchar* fsSourcePtr = fsString.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, nullptr);
	glShaderSource(fs, 1, &fsSourcePtr, nullptr);

	glCompileShader(vs);
	checkCompileErrors(vs, VERTEX);

	glCompileShader(fs);
	checkCompileErrors(fs, FRAGMENT);

	mHandle = glCreateProgram();
	glAttachShader(mHandle, vs);
	glAttachShader(mHandle, fs);
	glLinkProgram(mHandle);
	
	checkCompileErrors(mHandle, PROGRAM);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return true;
}


void ShaderProgram::use() {

	if (mHandle > 0) {
		glUseProgram(mHandle);
	} else {
		std::cerr << "Using ShaderProgram failed, mHandle is 0." << std::endl;
	}
}


std::string ShaderProgram::fileToString(const std::string& filename) 
{
	std::stringstream ss;
	std::ifstream file;

	try {
		file.open(filename, std::ios::in);

		if (!file.fail()) {
			ss << file.rdbuf();
		}

		file.close();
	}
	catch (std::exception ex) {
		throw std::runtime_error("Reading file: " + filename + " failed.");
	}

	return ss.str();
}


void ShaderProgram::checkCompileErrors(GLuint shader, ShaderType type) {
	int status = 0;

	if (type == PROGRAM) {
		glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length = 0;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetProgramInfoLog(mHandle, length, &length, &errorLog[0]);

			std::cerr << "ERROR: Shader program failed to link. " << errorLog << std::endl;
		}
	} else {
		// VERTEX or FRAGMENT shader
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);

			std::cerr << "ERROR: Shader failed to compile. " << errorLog << std::endl;
		}
	}
}


GLint ShaderProgram::getUniformLocation(const GLchar* name) {
	// Todo: Deal with scenario where user does not input valid uniform name.

	std::map<std::string, GLint>::iterator it = mUniformLocations.find(name);

	if (it == mUniformLocations.end()) {
		mUniformLocations[name] = glGetUniformLocation(mHandle, name);
	}

	return mUniformLocations[name];
}


void ShaderProgram::setUniform(const GLchar* name, const glm::vec2& vector) {
	GLint location = getUniformLocation(name);

	glUniform2f(location, vector.x, vector.y);
}


void ShaderProgram::setUniform(const GLchar* name, const glm::vec3& vector) {
	GLint location = getUniformLocation(name);

	glUniform3f(location, vector.x, vector.y, vector.z);
}


void ShaderProgram::setUniform(const GLchar* name, const glm::vec4& vector) {
	GLint location = getUniformLocation(name);

	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::mat4& matrix) {
	GLint location = getUniformLocation(name);

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
