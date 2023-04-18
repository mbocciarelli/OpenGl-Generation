#pragma once
#include "../Event/Event.h"
#include "../Layer/Layer.h"


class ImGuiLayer : public Layer
{
public:
	ImGuiLayer();
	~ImGuiLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnEvent(Event& e) override;

	void Begin();
	void End();

	void BlockEvents(bool block) { m_BlockEvents = block; }

	void SetDarkThemeColors();

	uint32_t GetActiveWidgetID() const;
private:
	bool m_BlockEvents = true;
};