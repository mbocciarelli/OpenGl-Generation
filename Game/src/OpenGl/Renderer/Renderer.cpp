#include "Renderer.h"
#include "RendererAPI.h"

std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	RendererAPI::Get()->SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(const Camera& camera)
{
	s_SceneData->ViewProjectionMatrix = camera.GetViewProjection();
	s_SceneData->ViewMatrix = camera.GetView();
	s_SceneData->ProjectionMatrix = camera.GetProjection();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
	shader->SetMat4("u_View", s_SceneData->ViewMatrix);
	shader->SetMat4("u_Projection", s_SceneData->ProjectionMatrix);
	shader->SetMat4("u_Transform", transform);

	vertexArray->Bind();
	RendererAPI::Get()->DrawIndexed(vertexArray);
}