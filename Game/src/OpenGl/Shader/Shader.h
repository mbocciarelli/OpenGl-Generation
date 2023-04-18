#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/fwd.hpp>

class Shader {
public:
    Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
    Shader(const std::string& name, const std::string& vertexShaderString, const std::string& fragmentShaderString);
    ~Shader();

    void Bind();
    void Unbind();

    void SetInt(const std::string& name, int value);

    void SetIntArray(const std::string& name, int* values, uint32_t count);

    void SetFloat(const std::string& name, float value);

    void SetFloat2(const std::string& name, const glm::vec2& value);

    void SetFloat3(const std::string& name, const glm::vec3& value);

    void SetFloat4(const std::string& name, const glm::vec4& value);

    void SetMat3(const std::string& name, const glm::mat3& matrix);

    void SetMat4(const std::string& name, const glm::mat4& matrix);

    static std::shared_ptr<Shader> Create(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    {
        return std::make_shared<Shader>(vertexShaderFilePath, fragmentShaderFilePath);
    }

    static std::shared_ptr<Shader> Create(const std::string& name, const std::string & vertexShaderFilePath, const std::string& fragmentShaderFilePath)
    {
        return std::make_shared<Shader>(name, vertexShaderFilePath, fragmentShaderFilePath);
    }

    std::string m_name;

private:
    GLuint CompileShader(const char* src, GLenum shaderType);

    GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);

    std::string ReadShaderFile(const std::string& filePath);

    GLint GetUniformLocation(const char* name) const;

    GLuint shaderProgram;

};

class ShaderLibrary
{
public:
    void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
    void Add(const std::shared_ptr<Shader>& shader);

    std::shared_ptr<Shader> Load(const std::string& name, const std::string& vertexShaderFilePath,
                                 const std::string& fragmentShaderFilePath);

    std::shared_ptr<Shader> Get(const std::string& name);

    bool Exists(const std::string& name) const;
private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
};