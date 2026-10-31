#include "ShaderManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderManager::ShaderManager() : m_programID(0) {}
ShaderManager::~ShaderManager() { if (m_programID) glDeleteProgram(m_programID); }

bool ShaderManager::loadShader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vFile, fFile;
    vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vFile.open(vertexPath);
        fFile.open(fragmentPath);
        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        vertexCode = vStream.str();
        fragmentCode = fStream.str();
        vFile.close();
        fFile.close();
    }
    catch (std::exception& e) {
        std::cerr << "ERROR::SHADER: Failed to read shader files: " << e.what() << std::endl;
        return false;
    }

    const char* vCode = vertexCode.c_str();
    const char* fCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    if (!checkCompileErrors(vertex, "VERTEX")) return false;

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);
    if (!checkCompileErrors(fragment, "FRAGMENT")) return false;

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    glLinkProgram(m_programID);
    if (!checkLinkErrors()) return false;

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

void ShaderManager::use() const { glUseProgram(m_programID); }
void ShaderManager::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void ShaderManager::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}
void ShaderManager::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}
void ShaderManager::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}

bool ShaderManager::checkCompileErrors(GLuint shader, const std::string& type) {
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
        return false;
    }
    return true;
}
bool ShaderManager::checkLinkErrors() {
    int success;
    char infoLog[1024];
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_programID, 1024, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << std::endl;
        return false;
    }
    return true;
}