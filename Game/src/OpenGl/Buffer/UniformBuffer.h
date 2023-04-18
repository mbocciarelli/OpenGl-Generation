#pragma once

#include <memory>
#include <GL/glew.h>

class UniformBuffer
{
public:
	UniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
	void SetData(const void* data, uint32_t size, uint32_t offset = 0)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

	static std::shared_ptr<UniformBuffer> Create(uint32_t size, uint32_t binding)
	{
		return std::make_shared<UniformBuffer>(size, binding);
	}
private:
	uint32_t m_RendererID = 0;
};