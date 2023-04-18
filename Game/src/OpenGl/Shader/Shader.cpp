#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath): m_name("")
{
	const std::string vertexShaderSrc = ReadShaderFile(vertexShaderFilePath);
	const std::string fragmentShaderSrc = ReadShaderFile(fragmentShaderFilePath);
	const GLuint vertexShader = CompileShader(vertexShaderSrc.c_str(), GL_VERTEX_SHADER);
	const GLuint fragmentShader = CompileShader(fragmentShaderSrc.c_str(), GL_FRAGMENT_SHADER);

    shaderProgram = CreateProgram(vertexShader, fragmentShader);
}

Shader::Shader(const std::string& name, const std::string& vertexShaderString, const std::string& fragmentShaderString): m_name(name)
{
    const GLuint vertexShader = CompileShader(vertexShaderString.c_str(), GL_VERTEX_SHADER);
    const GLuint fragmentShader = CompileShader(fragmentShaderString.c_str(), GL_FRAGMENT_SHADER);

    shaderProgram = CreateProgram(vertexShader, fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(shaderProgram);
}

void Shader::Bind() {
    glUseProgram(shaderProgram);
}

void Shader::Unbind() {
    glUseProgram(0);
}

GLint Shader::GetUniformLocation(const char* name) const
{
    return glGetUniformLocation(shaderProgram, name);
}

GLuint Shader::CompileShader(const char* src, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLsizei length;
        glGetShaderiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        auto* log = new GLchar(length);

        glGetShaderInfoLog(shader, length, &length, log);
        std::cerr << "Shader compilation failed" << log << std::endl;

        delete[] log;
        log = nullptr;
    }

    return shader;
}

GLuint Shader::CreateProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {

        GLsizei length;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
        auto* log = new GLchar(length + 1);

        glGetProgramInfoLog(shaderProgram, length, &length, log);
        std::cerr << "Shader program linking failed: " << log << std::endl;

        delete[] log;
        log = nullptr;

        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

std::string Shader::ReadShaderFile(const std::string& filePath)
{
    std::string result;
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return nullptr;
    }

    file.seekg(0, std::ios::end);
    const size_t size = file.tellg();
    if (size != -1)
    {
        result.resize(size);
        file.seekg(0, std::ios::beg);
        file.read(result.data(), size);
    }

    return result;
}

void Shader::SetInt(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name.c_str()), value);
}

void Shader::SetIntArray(const std::string& name, int* values, uint32_t count)
{
    glUniform1iv(GetUniformLocation(name.c_str()), count, values);
}

void Shader::SetFloat(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name.c_str()), value);
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    glUniform2f(GetUniformLocation(name.c_str()), value.x, value.y);
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    glUniform3f(GetUniformLocation(name.c_str()), value.x, value.y, value.z);
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    glUniform4f(GetUniformLocation(name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& matrix)
{
    glUniformMatrix3fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

// Shader library

void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
{
    //static_assert(!Exists(name), "Shader already exists!");
    m_Shaders[name] = shader;
}

void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
{
    if (!shader->m_name.empty())
		m_Shaders[shader->m_name] = shader;
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
	auto shader = Shader::Create(vertexShaderFilePath, fragmentShaderFilePath);
	Add(name, shader);
	return shader;
}

std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
{
    return m_Shaders[name];
}

bool ShaderLibrary::Exists(const std::string& name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}