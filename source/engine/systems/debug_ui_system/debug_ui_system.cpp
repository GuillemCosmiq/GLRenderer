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

#include "debug_ui_system.h"
#include "panels/base_panel.h"
#include "panels/render_panel.h"
#include "panels/performance_panel.h"
#include "panels/console_panel.h"
#include "../../engine.h"
#include "../../config.h"
#include "../input.h"
#include "../console.h"

namespace_begin

DebugUISystem::DebugUISystem()
	: m_enabled(false)
	, m_enabledInputHandler(0)
{
}

void DebugUISystem::Initialize(Config& config)
{
	Json::Value root = ReadJsonFromFile("../data/configs/" + config.GetDebugUiConfigPath());
	m_enabled = root["enabled"].asBool();
	
	bool error;
	error = AddPanel<RenderPanel>().expired();
	assert(error == false);
	error = AddPanel<PerformancePanel>().expired();
	assert(error == false);
	error = AddPanel<ConsolePanel>().expired();
	assert(error == false);

	m_enabledInputHandler = Engine::Get()->input->keyboardEmitter.AddReceptor([this](const SDL_Keycode key, const Input::KeyState state)
		{
			if (key == SDL_SCANCODE_F1 && state == Input::KeyState::down)
			{
				m_enabled = !m_enabled;
			}
		});

	Engine::Get()->console->AddCommand("EnableDebugUI", [this](int enabled) mutable
		{			
			//this->m_enabled = enabled;
			return "it worked!";
		});
}

void DebugUISystem::Update()
{
	if (m_enabled)
	{
		UpdateMainMenu();
		UpdatePanels();
	}
}

void DebugUISystem::Shutdown()
{
	Engine::Get()->input->keyboardEmitter.RemoveListener(m_enabledInputHandler);
	m_panels.clear();
}

void DebugUISystem::UpdateMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::EndMainMenuBar();
	}
}

void DebugUISystem::UpdatePanels()
{
	for (auto& panel : m_panels)
	{
		panel.second->Update();
	}
}

namespace_end
