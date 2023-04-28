#pragma once
#include <memory>
#include <gl/glew.h>

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
	{
        Load(indices, count);
    }

    void Load(const uint32_t *indices, uint32_t count)
    {
        glCreateBuffers(1, &m_RendererID);
        // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
        // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    ~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void SetData(uint32_t* indices, uint32_t count)
	{
		m_Count = count;
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	uint32_t GetCount() const { return m_Count; }

	static std::shared_ptr<IndexBuffer> Create(uint32_t* indices, uint32_t count)
	{
		return std::make_shared<IndexBuffer>(indices, count);
	}

private:
	uint32_t m_RendererID;
	uint32_t m_Count;
};
