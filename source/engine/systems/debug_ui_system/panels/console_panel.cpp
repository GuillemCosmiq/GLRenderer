// Copyright (c) 2019 Guillem Costa Miquel, kayter72@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. The above copyright notice and this permission notice shall be included in
//	  all copies or substantial portions of the Software.

#include "console_panel.h"

#include "../../../engine.h"
#include "../../renderer/renderer.h"
#include "../../input.h"
#include "../../console.h"

namespace_begin

ConsolePanel::ConsolePanel()
	: BasePanel()
	, m_consolePtr(nullptr)
	, m_show(false)
{
	m_consolePtr = Engine::Get()->console.get();
	m_showInputHandler = Engine::Get()->input->keyboardEmitter.AddReceptor([this](const SDL_Keycode key, const Input::KeyState state)
		{
			Show(key, state);
		});
}

ConsolePanel::ConsolePanel(const std::string& name)
	: BasePanel(name)
	, m_show(false)
{	
	m_consolePtr = Engine::Get()->console.get();
	m_showInputHandler = Engine::Get()->input->keyboardEmitter.AddReceptor([this](const SDL_Keycode key, const Input::KeyState state)
		{
			Show(key, state);
		});
}

void ConsolePanel::Update()
{
	if (!m_show)
		return;

	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	glm::vec2 viewport = Engine::Get()->renderer->GetViewport();
	ImGui::SetNextWindowSize(ImVec2(viewport.x, viewport.y / 3.f));
	ImGui::SetNextWindowFocus();
	if (ImGui::Begin(m_name.c_str(), nullptr, ImGuiWindowFlags_NoDecoration))
	{
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear")) {}
			ImGui::EndPopup();
		}
		ImGuiListClipper clipper(m_consolePtr->GetOutputs().size());
		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				ImGui::TextColored(m_consolePtr->GetOutputs()[i].first , m_consolePtr->GetOutputs()[i].second.c_str());
			}
		}
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::Separator();
		ImGui::Text(">");
		ImGui::SameLine();
		ImGui::SetKeyboardFocusHere();
		std::string inputString;
		// We have to request the string to reserve an arbitrary amount of memory because imgui uses capacity to determine max string size
		inputString.reserve(50);
		if (ImGui::InputText("", &inputString
			, ImGuiInputTextFlags_EnterReturnsTrue
			| ImGuiInputTextFlags_CallbackHistory
			| ImGuiInputTextFlags_CallbackCompletion))
		{
			m_consolePtr->ParseString(inputString, std::string(), std::vector<std::string>());
		}
	}
	ImGui::End();
}

void ConsolePanel::Show(const SDL_Keycode key, const Input::KeyState state)
{
	if (key == SDL_SCANCODE_GRAVE && state == Input::KeyState::down)
	{
		m_show = !m_show;
	}
}

namespace_end
