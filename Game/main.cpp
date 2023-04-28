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

#include "../ProceduralGeneration/PerlinNoise/PerlinGeneration.hpp"
#include <glm/gtc/noise.hpp>
#include "../ProceduralGeneration/Terrain/HeightMap/HeightMap.h"
#include "../ProceduralGeneration/Terrain/Terrain.h"
#include "../ProceduralGeneration/Terrain/Erosion/Erosion.h"
#include <queue>
#include <thread>

class TestLayer : public Layer
{
public:
	TestLayer(int width, int height) : Layer("TestLayer"), m_cameraController(45.f, (float)width / float(height), 0.1f, 1000.f)
	{
		m_cameraController.GetCamera().SetPosition({ -37.5531f, 71.7751f, 6.45213f });
		m_cameraController.GetCamera().SetYaw(33.f);

		GenerateChunks();
        /*std::queue<Chunk> m_queueChunks;
        std::vector<std::thread> m_threads;

        for (int x = 0; x < m_chunkSizeX; ++x) {
            for (int z = 0; z < m_chunkSizeZ; ++z) {
                Chunk chunk { x, z, m_mapSize, m_mapSize };
                m_queueChunks.push(chunk);
            }
        }

        for(int i = 0; i < nbrThreads; ++i)
        {
            m_threads.emplace_back([&](){
                while(!m_queueChunks.empty())
                {
                    m_mutex.lock();
                    auto& chunk = m_queueChunks.front();
                    m_queueChunks.pop();
                    m_mutex.unlock();

                    std::cout << "Generating chunk: " << chunk.x << " " << chunk.z << std::endl;
                    GenerateChunk(chunk, true);

                    m_mutex.lock();
                    m_chunks.push_back(chunk);
                    m_mutex.unlock();
                }
            });
        }

        for(auto& thread : m_threads)
        {
            thread.join();
        }

        for(auto& chunk : m_chunks)
        {
            RegenerationVerticesIndices(chunk, chunk.vertices, chunk.indices, true);
        }*/

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
		textureShader->SetFloat("u_minHeight", m_continalnessNoiseSettings.minHeight);
		textureShader->SetFloat("u_maxHeight", m_continalnessNoiseSettings.maxHeight);

		glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0.5f, 1.0f, 0.0f));

		for (const auto& chunk: m_chunks)
        {
            Renderer::Submit(textureShader, chunk.vertexArray, model);
        }

		Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		auto& io = ImGui::GetIO();

		bool mapHasBeenUpdated = false;
		bool sizeHasChanged = false;

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

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("DEBUG")) {
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("MAP OPTIONS")) {

					ImGui::Checkbox("Generate map", &m_generateMap);

					sizeHasChanged |= ImGui::SliderInt("Chunk Size", &m_chunkSize, 10.f, 5000.0f);
					mapHasBeenUpdated |= sizeHasChanged;
					mapHasBeenUpdated |= ImGui::Checkbox("Add Erosion", &m_erosionSettings.enable);

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Continentalness")) {
					if (ImGui::BeginTabBar("ContinentalnessBar")) {
						if (ImGui::BeginTabItem("General options")) {

							mapHasBeenUpdated |= ImGui::SliderFloat("min height", &m_continalnessNoiseSettings.minHeight, 0, 256.f);
							mapHasBeenUpdated |= ImGui::SliderFloat("max height", &m_continalnessNoiseSettings.maxHeight, 0, 256.f);

							mapHasBeenUpdated |= ImGui::Checkbox("Add Ridge Noise", &m_continalnessNoiseSettings.ridgeNoise);

							ImGui::EndTabItem();
						}

						if (ImGui::BeginTabItem("Continentalness options")) {

							mapHasBeenUpdated |= ImGui::SliderFloat("Frequency", &m_continalnessNoiseSettings.frequency, 0.0f, 10.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Octaves", &m_continalnessNoiseSettings.octaves, 1, 10);
							mapHasBeenUpdated |= ImGui::SliderFloat("Persistence", &m_continalnessNoiseSettings.persistence, 0.1f, 1.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Seed", &m_continalnessNoiseSettings.seed, 0, 100000);
							mapHasBeenUpdated |= ImGui::SliderFloat("Exponent", &m_continalnessNoiseSettings.exponent, 0.8f, 10.0f);

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
								ImGui::InputFloat("##Continentalness", &m_continalnessNoiseSettings.splinePoints[i].value);

								ImGui::PopItemWidth();
								ImGui::NextColumn();

								ImGui::PushItemWidth(-1);

								// Draw the height input box for this row
								ImGui::InputFloat("##Height", &m_continalnessNoiseSettings.splinePoints[i].height);

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
							if (m_heightMapTest.textureId != 0)
								ImGui::Image((void*)(intptr_t)m_heightMapTest.textureId, ImVec2(m_heightMapTest.mapWidth, m_heightMapTest.mapHeight));
							ImGui::EndTabItem();
						}

						ImGui::EndTabBar();
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Erosion")) {
					if (ImGui::BeginTabBar("ErosionBar")) {
						if (ImGui::BeginTabItem("General options")) {

							mapHasBeenUpdated |= ImGui::SliderFloat("min height", &m_erosionNoiseSettings.minHeight, 0, 256.f);
							mapHasBeenUpdated |= ImGui::SliderFloat("max height", &m_erosionNoiseSettings.maxHeight, 0, 256.f);

							mapHasBeenUpdated |= ImGui::Checkbox("Add Ridge Noise", &m_erosionNoiseSettings.ridgeNoise);

							ImGui::EndTabItem();
						}

						if (ImGui::BeginTabItem("Erosion options")) {

							mapHasBeenUpdated |= ImGui::SliderFloat("Frequency", &m_erosionNoiseSettings.frequency, 0.0f, 10.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Octaves", &m_erosionNoiseSettings.octaves, 1, 10);
							mapHasBeenUpdated |= ImGui::SliderFloat("Persistence", &m_erosionNoiseSettings.persistence, 0.1f, 1.0f);
							mapHasBeenUpdated |= ImGui::SliderInt("Seed", &m_erosionNoiseSettings.seed, 0, 100000);
							mapHasBeenUpdated |= ImGui::SliderFloat("Exponent", &m_erosionNoiseSettings.exponent, 0.8f, 10.0f);

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
								ImGui::InputFloat("##Erosion", &m_erosionNoiseSettings.splinePoints[i].value);

								ImGui::PopItemWidth();
								ImGui::NextColumn();

								ImGui::PushItemWidth(-1);

								// Draw the height input box for this row
								ImGui::InputFloat("##Height", &m_erosionNoiseSettings.splinePoints[i].height);

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


				if (m_erosionSettings.enable && ImGui::BeginTabItem("EROSION NOISE")) {
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
             /*std::queue<Chunk*> m_queueChunks;
             std::vector<std::thread> m_threads;*/

			 GenerateChunks();

             /*for(int i = 0; i < nbrThreads; ++i)
             {
                 m_threads.emplace_back([&](){
                     while(!m_queueChunks.empty())
                     {
                         m_mutex.lock();
                         auto chunk = *m_queueChunks.front();
                         m_queueChunks.pop();
                         m_mutex.unlock();

                         std::cout << "Generating chunk: " << chunk.x << " " << chunk.z << std::endl;
                         GenerateChunk(chunk, chunk.vertices, chunk.indices, sizeHasChanged);
                     }
                 });
             }

             for(auto& thread : m_threads)
                 thread.join();

             for(auto& chunk : m_chunks)
                 RegenerationVerticesIndices(chunk, chunk.vertices, chunk.indices, sizeHasChanged);*/
         }

	}

	void OnEvent(Event& e) override
	{
		m_cameraController.OnEvent(e);
	}


	void GenerateChunk(Chunk& chunk, bool sizeHasChanged)
	{
		Terrain terrain { chunk , m_continalnessNoiseSettings };

		auto& heightMap = terrain.GetHeightMap();
		m_heightMapTest = heightMap;
		m_heightMapTest.CreateHeightMapTexture();
		Erosion erosion{ m_erosionSettings };
		erosion.Erode(heightMap, m_chunkSize, m_erosionIterations);

		terrain.SetHeightMap(heightMap);
		terrain.GenerateVertices(chunk, m_continalnessNoiseSettings);
		chunk.vertices = terrain.GetVertices();
		chunk.indices = terrain.GetIndices();

		RegenerationVerticesIndices(chunk, sizeHasChanged);
	}

    void RegenerationVerticesIndices(Chunk& chunk, bool sizeHasChanged)
    {
        if (!sizeHasChanged) {

            if(chunk.vertexArray == nullptr)
                throw std::runtime_error("Chunk has no vertex array index");

            auto& vertexBuffer = chunk.vertexArray->GetVertexBuffers()[0];
            vertexBuffer->SetData(chunk.vertices.data(), sizeof(float) * chunk.vertices.size());

            auto& indexBuffer = chunk.vertexArray->GetIndexBuffer();
            indexBuffer->SetData(chunk.indices.data(), chunk.indices.size());

        }
        else
        {
            auto vertexArray = VertexArray::Create();
			chunk.vertexArray = vertexArray;

            const auto vertexBuffer = VertexBuffer::Create(chunk.vertices.data(), sizeof(float) * chunk.vertices.size());
            const BufferLayout layout = {
                    { ShaderDataType::Float3, "a_Position" },
            };
            vertexBuffer->SetLayout(layout);
            vertexArray->AddVertexBuffer(vertexBuffer);

            const auto indexBuffer = IndexBuffer::Create(chunk.indices.data(), chunk.indices.size());
            vertexArray->SetIndexBuffer(indexBuffer);
        }
    }


	void GenerateChunks()
	{
		m_chunks.clear();
		for (int x = 0; x < m_nbChunksX; ++x)
			for (int z = 0; z < m_nbChunksZ; ++z)
				m_chunks.emplace_back(Chunk{ x, z, m_chunkSize, m_chunkSize });

		for (auto& chunk : m_chunks) {
			GenerateChunk(chunk, true);
		}
	}

private:
	
    bool lauchRegenMap = false;
    std::mutex m_mutex;

	CameraController m_cameraController;

    std::vector<Chunk> m_chunks;
	std::shared_ptr<VertexArray> m_SquareVA;
	std::shared_ptr<Texture2D> m_texture;
	ShaderLibrary m_ShaderLibrary;

	NoiseSettings m_continalnessNoiseSettings;
	NoiseSettings m_erosionNoiseSettings;
	ErosionSettings m_erosionSettings;
	int m_erosionIterations = 0;

	bool m_generateMap = false;

	HeightMap m_heightMapTest;

    int m_chunkSize = 10;

	int m_nbChunksX = 3;
	int m_nbChunksZ = 3;

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
