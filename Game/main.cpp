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
#include "libs/noise/PerlinNoise.h"
#include <glm/gtc/noise.hpp>


struct NoiseSettings
{
	float x = 0.f;
	float y = 0.f;
	float frequency = 2.f;
	int octaves = 5;
	float lacunarity = 2.f;
	float persistence = 0.5f;
	float minHeight = 0.f;
	float maxHeight = 200.f;
	float exponent = 3.f;
	int seed = 0;
};

template<typename Type>
Type Map(Type val, Type in_min, Type in_max, Type out_min, Type out_max)
{
	return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void GenerateHeightMap(std::vector<float>& vertices, std::vector<uint32_t>& indices, int size, NoiseSettings& settings)
{
	const siv::PerlinNoise perlin(settings.seed);
	const float f = (settings.frequency / (float)size);
	for (int z = 0; z < size; ++z)
	{
		for (int x = 0; x < size; ++x)
		{
			size_t index = (x + z * size) * 3;
			float height = perlin.octave2D_01(x * f, z * f, settings.octaves, settings.persistence);
			height = pow(height, 3.f);
			vertices[index] = x;
			vertices[index + 1] = Map(height, 0.f, 1.f, settings.minHeight, settings.maxHeight);
			vertices[index + 2] = z;
		}
	}
	int indexIndex = 0;
	for (int z = 0; z < size - 1; ++z)
	{
		for (int x = 0; x < size - 1; ++x)
		{
			int topLeft = (x + z * size);
			int topRight = topLeft + 1;
			int bottomLeft = topLeft + size;
			int bottomRight = bottomLeft + 1;

			indices[indexIndex++] = topLeft;
			indices[indexIndex++] = bottomLeft;
			indices[indexIndex++] = topRight;
			indices[indexIndex++] = topRight;
			indices[indexIndex++] = bottomLeft;
			indices[indexIndex++] = bottomRight;
		}
	}
}

class TestLayer : public Layer
{
public:
	TestLayer(int width, int height) : Layer("TestLayer"), m_cameraController(45.f, (float)width / float(height), 0.1f, 1000.f)
	{
		m_cameraController.GetCamera().SetPosition({ -37.5531f, 71.7751f, 6.45213f });
		m_cameraController.GetCamera().SetYaw(33.f);

		m_vertexArray = VertexArray::Create();

		// Generate height map vertices
		std::vector<float> vertices;
		const size_t vertexCount = m_mapSize * m_mapSize;
		vertices.resize(vertexCount * 3);
		std::vector<uint32_t> indices;
		indices.resize(vertexCount * 6);

		GenerateHeightMap(vertices, indices, m_mapSize, m_noiseSettings);

		m_vertexArray->Bind();
		const auto vertexBuffer = VertexBuffer::Create(vertices.data(), sizeof(float) * vertices.size());
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
		};
		vertexBuffer->SetLayout(layout);
		m_vertexArray->AddVertexBuffer(vertexBuffer);

		auto indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
		m_vertexArray->SetIndexBuffer(indexBuffer);

		m_ShaderLibrary.Load("MapShader", "./assets/shaders/vertexShader.glsl", "./assets/shaders/fragmentShader.glsl");

		auto shader = m_ShaderLibrary.Get("MapShader");

		shader->Bind();
	}

	~TestLayer() override = default;

	void OnAttach() override {}
	void OnDetach() override {}

	void OnUpdate(float dt) override
	{
		m_cameraController.OnUpdate(dt);

		RendererAPI::Get()->SetClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		RendererAPI::Get()->Clear();

		Renderer::BeginScene(m_cameraController.GetCamera());

		const auto textureShader = m_ShaderLibrary.Get("MapShader");
		textureShader->SetFloat("u_minHeight", m_noiseSettings.minHeight);
		textureShader->SetFloat("u_maxHeight", m_noiseSettings.maxHeight);

		glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0.5f, 1.0f, 0.0f));
		Renderer::Submit(textureShader, m_vertexArray, model);

		Renderer::EndScene();

	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Perlin Noise Generator Settings");
		ImGui::SliderFloat("Frequency", &m_noiseSettings.frequency, 0.0f, 1.0f);
		ImGui::SliderInt("Octaves", &m_noiseSettings.octaves, 1, 10);
		ImGui::SliderFloat("Lacunarity", &m_noiseSettings.lacunarity, 1.0f, 5.0f);
		ImGui::SliderFloat("Persistence", &m_noiseSettings.persistence, 0.1f, 1.0f);
		ImGui::SliderInt("Seed", &m_noiseSettings.seed, 0, 100000);
		ImGui::SliderFloat("Exponent", &m_noiseSettings.exponent, 0.1f, 10.0f);

		ImGui::SliderFloat("min height", &m_noiseSettings.minHeight, -100.f, 300.0f);
		ImGui::SliderFloat("max height", &m_noiseSettings.maxHeight, -100.f, 300.0f);

		ImGui::End();


		ImGui::Begin("Camera");
		float m_speed = m_cameraController.GetCameraTranslationSpeed();
		ImGui::SliderFloat("Speed", &m_speed, -100.f, 300.0f);
		m_cameraController.SetCameraTranslationSpeed(m_speed);

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

	NoiseSettings m_noiseSettings;
	int m_mapSize = 500;
};


Application* CreateApplication()
{
	return new Application();
}

void main()
{
	Application* app = CreateApplication();
	glfwSetWindowSize(app->GetWindow().GetNativeWindow(), 800, 600);
	//glfwSetInputMode(app->GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int width, height;
	glfwGetWindowSize(app->GetWindow().GetNativeWindow(), &width, &height);
	app->PushLayer(new TestLayer(width, height));
	app->Run();

	delete app;
}
