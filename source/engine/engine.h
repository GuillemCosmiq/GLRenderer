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

#ifndef __ENGINE_H__
#define __ENGINE_H__

namespace_begin

class Config;
class SystemEvents;
class ResourceSystem;
class Renderer;
class Input;
class Console;
class DebugUISystem;
class Scene;

class Engine final : public Singleton<Engine>
{
public:
	explicit Engine(const std::string& configFileName);
	~Engine();

	void Run();
	void PreUpdate();
	void Update();
	void PostUpdate();

	void Quit();

private:
	bool Initialize();
	bool Shutdown();

public:
	// TODO: Allocate systems on heap. We dont want to overflow stack :)

	std::unique_ptr<Config> config;
	std::unique_ptr<SystemEvents> systemEvents;
	std::unique_ptr<ResourceSystem> resourceSystem;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Input> input;
	std::unique_ptr<Console> console;
	std::unique_ptr<DebugUISystem> debugUISystem;

	/* It should be an array of scenes or similar, just to be able to preload scenes.
	   But to keep it simple for the scope of the project I will let it to just one */
	std::shared_ptr<Scene> scene;

private:
	bool m_quit;
};

namespace_end

#endif