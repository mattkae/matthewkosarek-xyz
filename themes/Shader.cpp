#include "Shader.h"
#include <string>

GLuint loadIndividualShader(GLenum shaderType, const GLchar* cCode) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &cCode, 0);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, 0, infoLog);
		printf("Failed to load shader: %s, Shader =%s\n", infoLog, cCode);
        return 0;
    }
    
    return shader;
}

void attachShaders(Shader& retVal, const GLchar* vertexShader, const GLchar* fragmentShader) {
    GLuint vertex = 0, fragment = 0, geometry = 0;
    if (vertexShader) {
        vertex = loadIndividualShader(GL_VERTEX_SHADER, vertexShader);
        glAttachShader(retVal, vertex);
    }

    if (fragmentShader) {
        fragment = loadIndividualShader(GL_FRAGMENT_SHADER, fragmentShader);
        glAttachShader(retVal, fragment);
    }

    glLinkProgram(retVal);
    GLint isLinked = 0;
    glGetProgramiv(retVal, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(retVal, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        GLchar* infoLog = new GLchar[maxLength];
        glGetProgramInfoLog(retVal, maxLength, &maxLength, infoLog);
        glDeleteProgram(retVal);
        printf("Error. Could not initialize shader with vertex=%s, error=%s\n", vertexShader, infoLog);
        delete []infoLog;
    }

    if (vertexShader)
        glDeleteShader(vertex);
    if (fragmentShader)
        glDeleteShader(fragment);
}

Shader loadShader(const GLchar* vertexShader, const GLchar* fragmentShader) {
    Shader retVal;
    retVal = glCreateProgram();

    attachShaders(retVal, vertexShader, fragmentShader);
    useShader(retVal);

    return retVal;
}
