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

#include "system_events.h"

#include "../engine.h"
#include "renderer/renderer.h"
#include "input.h"

namespace_begin

SystemEvents::SystemEvents()
{
}

void SystemEvents::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		LOG(0, "Error initializing sdl");
	}
	SDL_GL_LoadLibrary(NULL);
}

void SystemEvents::PreUpdate()
{
	//camera.ProcessMouse((float)event.motion.xrel, (float)event.motion.yrel);
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		Engine::Get()->renderer->ImguiProcessEvent(event);
		switch (event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			Engine::Get()->input->HandleEvent(event);
			break;
		case SDL_WINDOWEVENT:
			Engine::Get()->renderer->HandleEvent(event);
			break;
		}
	}
}

void SystemEvents::Update()
{
}

void SystemEvents::PostUpdate()
{
}

namespace_end