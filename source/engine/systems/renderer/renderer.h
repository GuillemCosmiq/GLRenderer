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

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "../resource_system/resources/texture.h"
#include "../resource_system/resources/cubemap.h"

namespace_begin

class Config;
class GPUProfiler;
class ShadersStorage;
class ResourceSystem;
class Mesh;
class Texture;
class Cubemap;
class GeometryPass;
class EnvironmentPass;
class Lighting;
class ShadowMappingPass;
class PostProcessor;
class ScreenSampler;

class DrawableComponent;
class DirectionalLightComponent;
class PointLightComponent;
class CameraComponent;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Initialize(Config& config, ResourceSystem& resSystem);
	void PreUpdate();
	void PostUpdate();
	void Update(ResourceSystem& resSystem);
	void Shutdown();

	void ImguiProcessEvent(SDL_Event& SDLEvent);
	void HandleEvent(SDL_Event& SDLEvent);

	void AddDrawable(std::shared_ptr<DrawableComponent> drawable);
	void RemoveDrawable(std::shared_ptr<DrawableComponent> drawable);
	void AddLight(std::shared_ptr<DirectionalLightComponent> directionalLight);
	void RemoveLight(std::shared_ptr<DirectionalLightComponent> directionalLight);
	void AddLight(std::shared_ptr<PointLightComponent> pointLight);
	void RemoveLight(std::shared_ptr<PointLightComponent> pointLight);

	void SetCamera(std::shared_ptr<CameraComponent> camera);
	std::shared_ptr<CameraComponent> GetCamera() const { return m_camera; };

	void SetViewport(const glm::vec2& viewport);
	glm::vec2 GetViewport() const { return m_viewport; }
	const glm::mat4& GetPrevFrameProjViewMatrix() const { return m_prevProjViewMatrix; }
	GPUProfiler* GetGPUProfilerPtr() const { return m_profiler.get(); }

private:
	void Render();
	void UpdateCameraBlock() const;

public:
	std::unique_ptr<GeometryPass> geometryPass;
	std::unique_ptr<ShadowMappingPass> shadowMappingPass;
	std::unique_ptr<Lighting> lighting;
	std::unique_ptr<EnvironmentPass> environment;
	std::unique_ptr<PostProcessor> postProcessor;
	std::unique_ptr<ScreenSampler> screenSampler;
	std::unique_ptr<ShadersStorage> shaderStorage;

	Mesh* quad;
	Mesh* cube;

private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_GLContext m_context;

	glm::vec2 m_viewport;
	uint32 m_cameraBlockID;
	glm::mat4x4 m_prevProjViewMatrix;

	std::unique_ptr<GPUProfiler> m_profiler;
	std::shared_ptr<CameraComponent> m_camera;
	std::vector<std::shared_ptr<DrawableComponent>> m_drawables;
	std::vector<std::shared_ptr<DirectionalLightComponent>> m_directionalLights;
	std::vector<std::shared_ptr<PointLightComponent>> m_pointLights;
};

namespace_end;

#endif