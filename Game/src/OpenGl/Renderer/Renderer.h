#pragma once
#include "../../Camera/Camera.h"
#include "../Buffer/VertexArray.h"
#include "../Shader/Shader.h"

class OrthographicCamera;

class Renderer
{
public:
	static void Init();
	static void Shutdown();

	static void OnWindowResize(uint32_t width, uint32_t height);

	static void BeginScene(const Camera& camera);

	static void EndScene();

	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

private:
	struct SceneData
	{
		glm::mat4 ViewProjectionMatrix;
	};

	static std::unique_ptr<SceneData> s_SceneData;
};
