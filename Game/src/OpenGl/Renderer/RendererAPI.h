#pragma once

#include <glm/glm.hpp>

#include "../Buffer/VertexArray.h"

class RendererAPI
{
public:
	~RendererAPI() = default;

	void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	void SetClearColor(const glm::vec4& color);
	void Clear();
	void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0);

	static std::shared_ptr<RendererAPI> Get()
	{
		if (!s_instance)
			s_instance = Create();
		return s_instance;
	}

private:
	void Init();

	static std::shared_ptr<RendererAPI> Create()
	{
		auto rendererAPI = std::make_shared<RendererAPI>();
		rendererAPI->Init();
		return rendererAPI;
	}

	static std::shared_ptr<RendererAPI> s_instance;
};
