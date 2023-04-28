#pragma once
#include <cstdint>
#include <memory>
#include <gl/glew.h>
#include "Buffer.h"

class VertexBuffer
{
public:
	VertexBuffer(uint32_t size)
	{
		Load(size);
	}

	VertexBuffer(float* vertices, uint32_t size)
	{
        Load(size, vertices);
	}

    void Load(uint32_t size, float* vertices = nullptr)
    {
        glCreateBuffers(1, &m_id);
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

	~VertexBuffer()
	{
		glDeleteBuffers(1, &m_id);
	}

	void Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}

	void Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SetData(const void* data, const uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}


	const BufferLayout& GetLayout() const { return m_layout; }

	void SetLayout(const BufferLayout& layout) { m_layout = layout; }

	static std::shared_ptr<VertexBuffer> Create(float* vertices, uint32_t size)
	{
		return std::make_shared<VertexBuffer>(vertices, size);
	}

	static std::shared_ptr<VertexBuffer> Create(uint32_t size)
	{
		return std::make_shared<VertexBuffer>(size);
	}


private:
	GLuint m_id;
	BufferLayout m_layout;
};
