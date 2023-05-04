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

#include "src/PerlinNoise/PerlinGeneration.h"
#include <glm/gtc/noise.hpp>
#include "src/Terrain/HeightMap/HeightMap.h"
#include <queue>
#include <thread>
#include "src/Terrain/Chunk.h"
#include <glm/gtc/type_ptr.hpp>
#include "src/Terrain/Water/Water.h"
class TestLayer : public Layer
{
public:
	TestLayer(int width, int height) : Layer("TestLayer"), m_cameraController(45.f, (float)width / float(height), 0.1f, 1000.f), m_nbChunksX(5), m_nbChunksZ(5)
	{
		m_cameraController.GetCamera().SetPosition({ -37.5531f, 71.7751f, 6.45213f });
		m_cameraController.GetCamera().SetYaw(33.f);

		GenerateChunks();
		GenerateWater();
		
        m_ShaderLibrary.Load("MapShader", "./assets/shaders/vertexShader.glsl", "./assets/shaders/fragmentShader.glsl");

		m_ShaderLibrary.Load("WaterShader", "./assets/shaders/Water/vertexShader.glsl", "./assets/shaders/Water/fragmentShader.glsl");

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

		const auto mapShader = m_ShaderLibrary.Get("MapShader");
		const auto waterShader = m_ShaderLibrary.Get("WaterShader");
		glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0.5f, 1.0f, 0.0f));

		for (auto& chunk: m_chunks)
        {
            Renderer::Submit(mapShader, chunk.GetVertexArray(), model);
        }

		Renderer::Submit(waterShader, m_water.GetVertexArray(), model);

		Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		auto& io = ImGui::GetIO();

		bool mapHasBeenUpdated = false;
		bool sizeHasChanged = false;
		bool waterHeightUpdated = false;

		{
			ImGui::Begin("Debug");

			if (ImGui::BeginTabBar("MyTabBar")) {

				if (ImGui::BeginTabItem("FPS")) {
					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("CAMERA")) {
					float speed = m_cameraController.GetCameraTranslationSpeed();
					ImGui::SliderFloat("Speed", &speed, 0.f, 30000.0f);
					m_cameraController.SetCameraTranslationSpeed(speed);

					float far = m_cameraController.GetCamera().GetFar();
					ImGui::SliderFloat("Distance", &far, 0.f, 10000.0f);
					m_cameraController.GetCamera().SetFarClip(far);

					auto position = m_cameraController.GetCamera().GetPosition();
					float* data = glm::value_ptr(position);
					ImGui::InputFloat3("Position", data);
					glm::vec3 vec = glm::make_vec3(data);
					m_cameraController.GetCamera().SetPosition(vec);

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("DEBUG")) {
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("MAP OPTIONS")) {

					mapHasBeenUpdated |= ImGui::Checkbox("Generate map", &m_generateMap);
					mapHasBeenUpdated |= ImGui::Checkbox("Blend Noise map", &m_blendNoiseMap);

					sizeHasChanged |= ImGui::SliderInt("Chunk Size", &m_chunkSize, 10, 5000);
					sizeHasChanged |= ImGui::SliderInt("Chunk X", &m_nbChunksX, 1, 200);
					sizeHasChanged |= ImGui::SliderInt("Chunk Y", &m_nbChunksZ, 1, 200);

					sizeHasChanged |= ImGui::SliderInt("Chunk LOD", &m_lod, 1, 10);
					mapHasBeenUpdated |= sizeHasChanged;

					waterHeightUpdated |= ImGui::SliderInt("Water Height", &m_waterHeight, 0, 100);


					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Continentalness")) {
					if (ImGui::BeginTabBar("ContinentalnessBar")) {
						if (ImGui::BeginTabItem("General options")) {

							mapHasBeenUpdated |= ImGui::Checkbox("Add Ridge Noise", &m_continalnessNoiseSettings.ridgeNoise);

							ImGui::EndTabItem();
						}

						if (ImGui::BeginTabItem("Continentalness options")) {

							mapHasBeenUpdated |= ImGui::SliderFloat("Frequency", &m_continalnessNoiseSettings.frequency, 0.0f, 10.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Octaves", &m_continalnessNoiseSettings.octaves, 1, 10);
							mapHasBeenUpdated |= ImGui::SliderFloat("Persistence", &m_continalnessNoiseSettings.persistence, 0.1f, 1.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Seed", &m_continalnessNoiseSettings.seed, 0, 100000);

							ImGui::EndTabItem();
						}

						if (ImGui::BeginTabItem("Continentalness Noise Map Settings")) {

							ImGui::Columns(2, "ContinentalnessTable");
							ImGui::Text("Continentalness");
							ImGui::NextColumn();
							ImGui::Text("Height");
							ImGui::NextColumn();

							// Draw the table rows
							for (int i = 0; i < m_continalnessNoiseSettings.splinePoints.size(); i++) {
								ImGui::PushID(i);
								ImGui::PushItemWidth(-1);

								// Draw the noise input box for this row
								mapHasBeenUpdated |= ImGui::InputFloat("##Continentalness", &m_continalnessNoiseSettings.splinePoints[i].value);

								ImGui::PopItemWidth();
								ImGui::NextColumn();

								ImGui::PushItemWidth(-1);

								// Draw the z input box for this row
								mapHasBeenUpdated |= ImGui::InputFloat("##Height", &m_continalnessNoiseSettings.splinePoints[i].height);

								ImGui::PopItemWidth();
								ImGui::NextColumn();

								ImGui::PopID();
							}

							// Add a button to add a new row
							if (ImGui::Button("Add Row")) {
								// Create a new row and add it to the vector
								NoiseSettings::RowData newRow;
								m_continalnessNoiseSettings.splinePoints.push_back(newRow);
							}
							ImGui::EndTabItem();
						}

						if (ImGui::BeginTabItem("Continentalness Noise Image"))
						{

							if (m_continalnessNoiseHeightMap.textureId != 0)
								ImGui::Image((void*)(intptr_t)m_continalnessNoiseHeightMap.textureId, ImVec2(m_continalnessNoiseHeightMap.mapWidth, m_continalnessNoiseHeightMap.mapHeight));
							ImGui::EndTabItem();
						}

						ImGui::EndTabBar();
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Erosion")) {
					if (ImGui::BeginTabBar("ErosionBar")) {
						if (ImGui::BeginTabItem("General options")) {

							mapHasBeenUpdated |= ImGui::Checkbox("Add Ridge Noise", &m_erosionNoiseSettings.ridgeNoise);

							ImGui::EndTabItem();
						}

						if (ImGui::BeginTabItem("Erosion options")) {

							mapHasBeenUpdated |= ImGui::SliderFloat("Frequency", &m_erosionNoiseSettings.frequency, 0.0f, 10.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Octaves", &m_erosionNoiseSettings.octaves, 1, 10);
							mapHasBeenUpdated |= ImGui::SliderFloat("Persistence", &m_erosionNoiseSettings.persistence, 0.1f, 1.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Seed", &m_erosionNoiseSettings.seed, 0, 100000);
							mapHasBeenUpdated |= ImGui::SliderFloat("Erosion Factor", &m_erosionNoiseSettings.factor, 0.f, 1.f);

							ImGui::EndTabItem();
						}

						if (ImGui::BeginTabItem("Erosion Noise Map Settings")) {

							ImGui::Columns(2, "ErosionTable");
							ImGui::Text("Erosion");
							ImGui::NextColumn();
							ImGui::Text("Height");
							ImGui::NextColumn();

							// Draw the table rows
							for (int i = 0; i < m_erosionNoiseSettings.splinePoints.size(); i++) {
								ImGui::PushID(i);
								ImGui::PushItemWidth(-1);

								// Draw the noise input box for this row
								mapHasBeenUpdated |= ImGui::InputFloat("##Erosion", &m_erosionNoiseSettings.splinePoints[i].value);

								ImGui::PopItemWidth();
								ImGui::NextColumn();

								ImGui::PushItemWidth(-1);

								// Draw the z input box for this row
								mapHasBeenUpdated |= ImGui::InputFloat("##Height", &m_erosionNoiseSettings.splinePoints[i].height);

								ImGui::PopItemWidth();
								ImGui::NextColumn();

								ImGui::PopID();
							}

							// Add a button to add a new row
							if (ImGui::Button("Add Row")) {
								// Create a new row and add it to the vector
								NoiseSettings::RowData newRow;
								m_erosionNoiseSettings.splinePoints.push_back(newRow);
							}

							ImGui::EndTabItem();
						}

						ImGui::EndTabBar();
					}


					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("PRE-CONFIG")) {
					/*if (ImGui::Button("Desert Config"))
					{
						m_continalnessNoiseSettings = DesertConfig;
						mapHasBeenUpdated = true;
					}*/
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			ImGui::End();

		}


		 if (m_generateMap && mapHasBeenUpdated)
         {
			 GenerateChunks();
			 GenerateWater();
         }
		 if (waterHeightUpdated)
		 {
			 GenerateWater();
		 }

	}

	void OnEvent(Event& e) override
	{
		m_cameraController.OnEvent(e);
	}


	void GenerateChunk(Chunk& chunk, bool sizeHasChanged)
	{
		RegenerationVerticesIndices(chunk, sizeHasChanged);
	}

    void RegenerationVerticesIndices(Chunk& chunk, bool sizeHasChanged)
    {
        if (!sizeHasChanged) {

            if(chunk.GetVertexArray() == nullptr)
                throw std::runtime_error("Chunk has no vertex array index");

            auto& vertexBuffer = chunk.GetVertexArray()->GetVertexBuffers()[0];
            vertexBuffer->SetData(chunk.GetVertices().data(), sizeof(float) * chunk.GetVertices().size());

            auto& indexBuffer = chunk.GetVertexArray()->GetIndexBuffer();
            indexBuffer->SetData(chunk.GetIndices().data(), chunk.GetIndices().size());

        }
        else
        {
            auto vertexArray = VertexArray::Create();
			chunk.SetVertexArray(vertexArray);

            const auto vertexBuffer = VertexBuffer::Create(chunk.GetVertices().data(), sizeof(float) * chunk.GetVertices().size());
            const BufferLayout layout = {
                    { ShaderDataType::Float3, "a_Position" },
            };
            vertexBuffer->SetLayout(layout);
            vertexArray->AddVertexBuffer(vertexBuffer);

            const auto indexBuffer = IndexBuffer::Create(chunk.GetIndices().data(), chunk.GetIndices().size());
            vertexArray->SetIndexBuffer(indexBuffer);
        }
    }


	void GenerateChunks()
	{
		m_chunks.clear();
		std::vector<std::thread> threads;

		for (int x = 0; x < m_nbChunksX; ++x)
		{
			for (int z = 0; z < m_nbChunksZ; ++z)
			{
				threads.emplace_back([this, x, z] {
					Chunk newChunk{ x, z, m_chunkSize, m_chunkSize, m_lod, m_continalnessNoiseSettings, m_erosionNoiseSettings, m_blendNoiseMap };
					std::unique_lock<std::mutex> lock(mtx);
					m_chunks.emplace_back(std::move(newChunk));
					lock.unlock();
					});
			}
		}

		for (auto& thread : threads) {
			thread.join();
		}

		for (auto& chunk : m_chunks) {
			GenerateChunk(chunk, true);
		}
	}

	void GenerateWater()
	{
		m_water = Water(m_nbChunksX * (m_chunkSize - 1), m_nbChunksZ * (m_chunkSize - 1), m_waterHeight);
	}

private:
	
    bool lauchRegenMap = false;
    std::mutex m_mutex;

	CameraController m_cameraController;

    std::vector<Chunk> m_chunks;
	ShaderLibrary m_ShaderLibrary;

	NoiseSettings m_continalnessNoiseSettings = continentalnessNoiseSettings;
	NoiseSettings m_erosionNoiseSettings = erosionNoiseSettings;

	bool m_generateMap = true;
	bool m_blendNoiseMap = true;

	HeightMap m_continalnessNoiseHeightMap;
	HeightMap m_erosionNoiseHeightMap;

	Water m_water;

    int m_chunkSize = 16;
	int m_lod = 1;

	int m_nbChunksX;
	int m_nbChunksZ;

	int m_waterHeight = 40;

	std::mutex mtx;

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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int width, height;
	glfwGetWindowSize(app->GetWindow().GetNativeWindow(), &width, &height);
	app->PushLayer(new TestLayer(width, height));
	app->Run();

	delete app;
}
