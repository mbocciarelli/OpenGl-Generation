#pragma once
#include <memory>
#include "../Window/Window.h"
#include "../Layer/Layer.h"
#include "../Layer/LayerStack.h"
#include "../ImGui/ImGuiLayer.h"

class Application
{
public:
	Application();
	~Application();

	void Run();

	void PushLayer(Layer* layer);

	void PushOverlay(Layer* layer);

	void OnEvent(Event& e);

	static Application& Get();

	Window& GetWindow() const { return *m_window; }

	ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

private:
	static Application* s_instance;

	std::unique_ptr<Window> m_window;
	ImGuiLayer* m_ImGuiLayer;


	bool m_isRunning = true;

	LayerStack m_LayerStack;
	float m_LastFrameTime = 0.0f;

};

