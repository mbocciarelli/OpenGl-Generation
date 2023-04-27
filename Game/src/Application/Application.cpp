#include "Application.h"
#include "../Event/WindowEvent.h"
#include "../OpenGl/Renderer/Renderer.h"
#include "../Event/KeyEvent.h"

Application* Application::s_instance = nullptr;

Application::Application()
{
	if (s_instance)
	{
		throw std::runtime_error("Application already exists");
	}
	s_instance = this;

	m_window = Window::Create(WindowData(
	"Opengl2",
		1280,
		720
	));

	m_window->SetEventCallback([&](Event& e) { return Application::OnEvent(e); });

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);

}

Application::~Application()
{
	
}

void Application::Run()
{
	while (m_isRunning)
	{
		const auto time = static_cast<float>(glfwGetTime());
		const float dt = time - m_LastFrameTime;
		m_LastFrameTime = time;

		for (Layer* layer : m_LayerStack)
			layer->OnUpdate(dt);

		m_ImGuiLayer->Begin();
		{
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
		}
		m_ImGuiLayer->End();

		m_window->OnUpdate();
	}
}

void Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}

void Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);

	dispatcher.Dispatch<WindowResizeEvent>([](const WindowResizeEvent& event)
	{
		glViewport(0, 0, event.GetWidth(), event.GetHeight());
		return false;
	});

	dispatcher.Dispatch<WindowCloseEvent>([this](const WindowCloseEvent& event)
	{
		m_isRunning = false;
		return true;
	});

	dispatcher.Dispatch<KeyPressedEvent>([this](const KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == GLFW_KEY_ESCAPE) {
			m_isRunning = false;
			return true;
		}
	});


	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
	{
		(*--it)->OnEvent(e);
		if (e.Handled)
			break;
	}

}


Application& Application::Get()
{
	return *s_instance;
}


