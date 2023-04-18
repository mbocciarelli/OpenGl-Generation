#include <imgui.h>
#include <iostream>
#include "src/Application/Application.h"
#include "src/Camera/Camera.h"
#include "src/OpenGl/Buffer/VertexArray.h"
#include "src/OpenGl/Renderer/Renderer.h"
#include "src/OpenGl/Shader/Shader.h"
#include "src/OpenGl/Texture/Texture.h"

#include "src/Camera/CameraController.h"
#include "src/OpenGl/Renderer/RendererAPI.h"

class TestLayer : public Layer
{
public:
	TestLayer(int width, int height) : Layer("TestLayer"), m_cameraController(45.f, (float)width / float(height), 0.1f, 1000.f)
	{
		m_vertexArray = VertexArray::Create();

		float vertices[] = {
			// positions       // texture coords
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
		};


		auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[] = {
			6, 7, 3,3, 2, 6,
			4, 5, 1, 1, 0, 4,
			0, 1, 2, 2, 3, 0,
			1, 5, 6, 6, 2, 1,
			5, 4, 7, 7, 6, 5,
			4, 0, 3, 3, 7, 4,
		};

		auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_vertexArray->SetIndexBuffer(indexBuffer);

		m_ShaderLibrary.Load("Cube", "./assets/shaders/vertexShader.glsl", "./assets/shaders/fragmentShader.glsl");

		auto shader = m_ShaderLibrary.Get("Cube");
		shader->Bind();
		m_texture = Texture2D::Create("./assets/textures/texture.jpg");
	}

	virtual ~TestLayer() = default;

	void OnAttach() override {}
	void OnDetach() override {}

	void OnUpdate(float dt) override
	{
		m_cameraController.OnUpdate(dt);

		RendererAPI::Get()->SetClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		RendererAPI::Get()->Clear();

		Renderer::BeginScene(m_cameraController.GetCamera());

		const auto textureShader = m_ShaderLibrary.Get("Cube");
		m_texture->Bind();

		glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		Renderer::Submit(textureShader, m_vertexArray, model);

		Renderer::EndScene();

	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", 10);
		ImGui::Text("Quads: %d", 5);
		ImGui::Text("Vertices: %d", 2);
		ImGui::Text("Indices: %d", 0);

		ImGui::End();
	}

	void OnEvent(Event& e) override
	{
		m_cameraController.OnEvent(e);
	}

private:
	CameraController m_cameraController;

	std::shared_ptr<VertexArray> m_vertexArray;
	std::shared_ptr<VertexArray> m_SquareVA;

	std::shared_ptr<Texture2D> m_texture;
	ShaderLibrary m_ShaderLibrary;
};


Application* CreateApplication()
{
	return new Application();
}

void main()
{
	Application* app = CreateApplication();
	glfwSetWindowSize(app->GetWindow().GetNativeWindow(), 800, 600);
	glfwSetInputMode(app->GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int width, height;
	glfwGetWindowSize(app->GetWindow().GetNativeWindow(), &width, &height);
	app->PushLayer(new TestLayer(width, height));
	app->Run();

	delete app;
}
