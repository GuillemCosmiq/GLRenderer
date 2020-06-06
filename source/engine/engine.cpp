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

#include "engine.h"

#include "config.h"
#include "systems/system_events.h"
#include "systems/resource_system/resource_system.h"
#include "systems/renderer/renderer.h"
#include "systems/input.h"
#include "systems/console.h"
#include "systems/debug_ui_system/debug_ui_system.h"
#include "scene.h"

namespace_begin

Engine::Engine(const std::string& configFileName)
	: Singleton<Engine>(this)
	, config(new Config(configFileName))
	, systemEvents(new SystemEvents())
	, resourceSystem(new ResourceSystem())
	, renderer(new Renderer())
	, input(new Input())
	, console(new Console())
	, debugUISystem(new DebugUISystem())
	, scene(new Scene())
	, m_quit(false)
{
}

Engine::~Engine()
{
}

void Engine::Run()
{
	Initialize();

	while (!m_quit)
	{
		PreUpdate();
		Update();
		PostUpdate();
	}

	Shutdown();
}

void Engine::PreUpdate()
{
	systemEvents->PreUpdate();
	renderer->PreUpdate();
}

void Engine::Update()
{
	input->Update();
	scene->Update();
	debugUISystem->Update();
	renderer->Update(*resourceSystem);
}

void Engine::PostUpdate()
{
	renderer->PostUpdate();
}

void Engine::Quit()
{
	m_quit = true;
}

bool Engine::Initialize()
{
	bool ret = true;

	systemEvents->Initialize();
	resourceSystem->Initialize(*config);
	renderer->Initialize(*config, *resourceSystem);
	debugUISystem->Initialize(*config);
	scene->Initialize(*config, *resourceSystem);

	return ret;
}

bool Engine::Shutdown()
{
	bool ret = true;

	resourceSystem->Shutdown();
	debugUISystem->Shutdown();
	renderer->Shutdown();

	SDL_Quit();

	return ret;
}

namespace_end
