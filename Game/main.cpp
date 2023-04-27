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
#include <glm/gtc/noise.hpp>
#include "src/Terrain/HeightMap/HeightMap.h"
#include "src/Terrain/Terrain.h"
#include "src/Terrain/Erosion/Erosion.h"


static const NoiseSettings DesertConfig = {
	.x = 0.f,
	.y = 0.f,
	.frequency = 3.f,
	.octaves = 3,
	.persistence = 0.38f,
	.minHeight = 0.f,
	.maxHeight = 200.f,
	.exponent = 1.f,
	.seed = 0,
	.ridgeNoise = true
};


class TestLayer : public Layer
{
public:
	TestLayer(int width, int height) : Layer("TestLayer"), m_cameraController(45.f, (float)width / float(height), 0.1f, 1000.f)
	{
		m_cameraController.GetCamera().SetPosition({ -37.5531f, 71.7751f, 6.45213f });
		m_cameraController.GetCamera().SetYaw(33.f);

		RegenerateMap(true);

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
		bool mapHasBeenUpdated = false;
		bool sizeHasChanged = false;

		{
			ImGui::Begin("Map");

			mapHasBeenUpdated |= ImGui::SliderInt("Size", &m_mapSize, 10.f, 10000.0f);

			mapHasBeenUpdated |= ImGui::Checkbox("Add Ridge Noise", &m_noiseSettings.ridgeNoise);
			mapHasBeenUpdated |= ImGui::Checkbox("Add Terraces", &m_noiseSettings.terraces);

			sizeHasChanged |= mapHasBeenUpdated;
			//ImGui::SliderInt("Height", &m_height, 10.f, 10000.0f);

			ImGui::End();
		}

		{
			ImGui::Begin("Perlin Noise Generator Settings");
			mapHasBeenUpdated |= ImGui::SliderFloat("Frequency", &m_noiseSettings.frequency, 0.0f, 10.0f);
			mapHasBeenUpdated |= ImGui::SliderInt("Octaves", &m_noiseSettings.octaves, 1, 10);
			mapHasBeenUpdated |= ImGui::SliderFloat("Persistence", &m_noiseSettings.persistence, 0.1f, 1.0f);
			mapHasBeenUpdated |= ImGui::SliderInt("Seed", &m_noiseSettings.seed, 0, 100000);
			mapHasBeenUpdated |= ImGui::SliderFloat("Exponent", &m_noiseSettings.exponent,	0.8f, 10.0f);

			mapHasBeenUpdated |= ImGui::SliderFloat("min height", &m_noiseSettings.minHeight, -100.f, 300.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("max height", &m_noiseSettings.maxHeight, -100.f, 300.0f);

			if (m_noiseSettings.terraces)
			{
				mapHasBeenUpdated |= ImGui::SliderInt("Terraces", &m_noiseSettings.terraceCount, 1, 30);
			}


			ImGui::End();
		}

		{
			ImGui::Begin("Erosion Settings");

			mapHasBeenUpdated |= ImGui::SliderInt("Erosion Iterations", &m_erosionIterations, 0, 500000);

			mapHasBeenUpdated |= ImGui::SliderInt("seed", &m_erosionSettings.seed, 0, 100000);
			mapHasBeenUpdated |= ImGui::SliderInt("erosionRadius", &m_erosionSettings.erosionRadius, 1, 7);
			mapHasBeenUpdated |= ImGui::SliderFloat("Inertia", &m_erosionSettings.inertia, 0.0f, 1.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("sedimentCapacityFactor", &m_erosionSettings.sedimentCapacityFactor, 0.0f, 10.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("minSedimentCapacity", &m_erosionSettings.minSedimentCapacity, 0.0f, 1.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("erodeSpeed", &m_erosionSettings.erodeSpeed, 0.0f, 1.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("depositSpeed", &m_erosionSettings.depositSpeed, 0.0f, 1.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("evaporateSpeed", &m_erosionSettings.evaporateSpeed, 0.0f, 1.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("gravity", &m_erosionSettings.gravity, 1.0f, 10.0f);
			mapHasBeenUpdated |= ImGui::SliderInt("maxDropletLifetime", &m_erosionSettings.maxDropletLifetime, 1, 100);
			mapHasBeenUpdated |= ImGui::SliderFloat("initialWaterVolume", &m_erosionSettings.initialWaterVolume, 1.0f, 10.0f);
			mapHasBeenUpdated |= ImGui::SliderFloat("initialSpeed", &m_erosionSettings.initialSpeed, 0.0f, 3.0f);

			ImGui::End();

		}


		{
			ImGui::Begin("Camera");

			float speed = m_cameraController.GetCameraTranslationSpeed();
			ImGui::SliderFloat("Speed", &speed, 0.f, 30000.0f);
			m_cameraController.SetCameraTranslationSpeed(speed);


			float far = m_cameraController.GetCamera().GetFar();
			ImGui::SliderFloat("Distance", &far, 0.f, 10000.0f);
			m_cameraController.GetCamera().SetFarClip(far);


			ImGui::End();
		}

		{
			ImGui::Begin("PRE CONFIG");

			if (ImGui::Button("Desert Config"))
			{
				m_noiseSettings = DesertConfig;
				mapHasBeenUpdated = true;
			}

			ImGui::End();
		}

		if (mapHasBeenUpdated) RegenerateMap(sizeHasChanged);

	}

	void OnEvent(Event& e) override
	{
		m_cameraController.OnEvent(e);
	}


	void RegenerateMap(bool sizeHasChanged)
	{
		Terrain terrain { m_mapSize, m_mapSize, m_noiseSettings };

		auto& heightMap = terrain.GetHeightMap();

		Erosion erosion{ m_erosionSettings };
		erosion.Erode(heightMap, m_mapSize, m_erosionIterations);

		terrain.SetHeightMap(heightMap);
		terrain.GenerateVertices(m_mapSize, m_mapSize, m_noiseSettings);
		auto& vertices = terrain.GetVertices();
		auto& indices = terrain.GetIndices();

		if (!sizeHasChanged) {

			auto& vertexBuffer = m_vertexArray->GetVertexBuffers()[0];
			vertexBuffer->SetData(vertices.data(), sizeof(float) * vertices.size());

			auto& indexBuffer = m_vertexArray->GetIndexBuffer();
			indexBuffer->SetData(indices.data(), indices.size());
		} else
		{
			m_vertexArray = VertexArray::Create();

			const auto vertexBuffer = VertexBuffer::Create(vertices.data(), sizeof(float) * vertices.size());
			const BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" },
			};
			vertexBuffer->SetLayout(layout);
			m_vertexArray->AddVertexBuffer(vertexBuffer);

			const auto indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
			m_vertexArray->SetIndexBuffer(indexBuffer);
		}

		
	}


private:
	CameraController m_cameraController;

	std::shared_ptr<VertexArray> m_vertexArray;
	std::shared_ptr<VertexArray> m_SquareVA;
	std::shared_ptr<Texture2D> m_texture;
	ShaderLibrary m_ShaderLibrary;

	NoiseSettings m_noiseSettings;
	ErosionSettings m_erosionSettings;
	int m_erosionIterations = 100000;

	int m_mapSize = 500;
};


Application* CreateApplication()
{
	return new Application();
}

void main()
{
	Application* app = CreateApplication();
	glfwSetWindowSize(app->GetWindow().GetNativeWindow(), 1280, 720);
	//glfwSetInputMode(app->GetWindow().GetNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int width, height;
	glfwGetWindowSize(app->GetWindow().GetNativeWindow(), &width, &height);
	app->PushLayer(new TestLayer(width, height));
	app->Run();

	delete app;
}
