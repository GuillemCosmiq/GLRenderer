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

#include "input.h"

namespace_begin

Input::Input()
{
	std::fill(std::begin(m_keyboard), std::end(m_keyboard), KeyState::idle);
}

void Input::Update()
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < NumKeyboardKeys; ++i)
	{
		KeyState& currentKey = m_keyboard[i];
		if (keys[i] == 1)
		{
			if (currentKey == KeyState::idle)
			{
				currentKey = KeyState::down;
				keyboardEmitter.Emit(i, KeyState::down);
			}
			else
			{
				currentKey = KeyState::repeat;
				keyboardEmitter.Emit(i, KeyState::repeat);
			}
		}
		else
		{
			if (currentKey == KeyState::repeat || currentKey == KeyState::down)
			{
				currentKey = KeyState::up;
				keyboardEmitter.Emit(i, KeyState::up);
			}
			else
			{
				currentKey = KeyState::idle;
			}
		}
	}

	const Uint32 mouse = SDL_GetMouseState(NULL, NULL);
}

void Input::HandleEvent(SDL_Event& SDLEvent)
{
	switch (SDLEvent.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		if (SDLEvent.button.button == SDL_BUTTON_RIGHT)
			rightButtPressed = true;
		break;
	case SDL_MOUSEBUTTONUP:
		if (SDLEvent.button.button == SDL_BUTTON_RIGHT)
			rightButtPressed = false;
		break;
	case SDL_MOUSEMOTION:
	{
		if (rightButtPressed)
		{
			MousePayload payload;
			payload.deltaX = SDLEvent.motion.xrel;
			payload.deltaY = SDLEvent.motion.yrel;
			mouseEmitter.Emit(payload);
		}
		break;
	}
	}
}

namespace_end