#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include "mathlib.h"

typedef GLuint Shader;

Shader loadShader(const GLchar* vertexShader, const GLchar* fragmentShader);

inline GLint getShaderUniform(const Shader& shader, const GLchar *name) {
    GLint uid = glGetUniformLocation(shader, name);
	if (uid < 0) {
		return -1;
	}
	return uid;
}

inline GLint getShaderAttribute(const Shader& shader, const GLchar *name) {
	printf("Getting attribute for shader, name: %d, %s\n", shader, name);
    GLint uid = glGetAttribLocation(shader, name);
	if (uid < 0) {
		printf("Unable to get attribute %s for shader %d\n", name, shader);
		return -1;
	}
	return uid;
}

inline void useShader(const Shader& shader) {
	glUseProgram(shader);
}

inline void setShaderFloat(GLint location, GLfloat value) {
    glUniform1f(location, value);
}

inline void setShaderInt(GLint location, GLint value) {
	glUniform1i(location, value);
}

inline void setShaderUint(GLint location, GLuint value) {
	glUniform1ui(location, value);
}

inline void setShaderVec2(GLint location, const Vector2& value) {
	glUniform2f(location, value.x, value.y);
}

inline void setShaderMat4(GLint location, const Mat4x4& matrix) {
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix.m);
}

inline void setShaderBVec3(GLint location, bool first, bool second, bool third) {
	glUniform3i(location, first, second, third);
}

inline void setShaderBVec4(GLint location, bool first, bool second, bool third, bool fourth) {
	glUniform4i(location, first, second, third, fourth);
}

inline void setShaderBool(GLint location, bool value) {
	glUniform1i(location, value);
}
