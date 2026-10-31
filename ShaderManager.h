#pragma once
#include "Common.h"
#include <string>

class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    bool loadShader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

    GLuint getID() const { return m_programID; }

private:
    GLuint m_programID;
    bool checkCompileErrors(GLuint shader, const std::string& type);
    bool checkLinkErrors();
};
