#pragma once

#include <vector>
#include "src/OpenGl/Buffer/VertexArray.h"

class Water
{
public:
	Water() = default;
	Water(int x, int z, int height): x(x), z(z), h(height)
	{
		std::vector<float> vertices = {
			0.f, (float)h, (float)z,
			(float)x, (float)h, (float)z,
			0.f, (float)h, 0.f,
			(float)x, (float)h, 0.f,
		};

		std::vector<uint32_t> indices = {
			0, 1, 2,
			2, 1, 3
		};

		m_vertexArray = VertexArray::Create();

		const auto vertexBuffer = VertexBuffer::Create(vertices.data(), sizeof(float) * vertices.size());
		const BufferLayout layout = {
			{
				ShaderDataType::Float3, "a_Position"
			},
		};
		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		const auto indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
		m_vertexArray->SetIndexBuffer(indexBuffer);
	}

	std::shared_ptr<VertexArray>& GetVertexArray()
	{
		return m_vertexArray;
	}

	

private:
	int x;
	int z;
	int h;
	std::shared_ptr<VertexArray> m_vertexArray;
};