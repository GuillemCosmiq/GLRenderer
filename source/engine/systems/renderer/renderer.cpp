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

#include "renderer.h"

#include "gpu_profiler.h"
#include "shaders_storage.h"

#include "../../config.h"
#include "../../engine.h"

#include "render_passes/geometry_pass.h"
#include "render_passes/environment_pass.h"
#include "render_passes/lighting.h"
#include "render_passes/shadow_mapping_pass.h"
#include "render_passes/postprocessor.h"
#include "render_passes/screen_sampler.h"

#include "render_sources/gbuffer_source.h"
#include "render_sources/geometry_source.h"
#include "render_sources/lighting_source.h"
#include "render_sources/shadow_mapping_pass_source.h"
#include "render_sources/postprocessor_source.h"
#include "render_sources/screen_sampler_source.h"

#include "../resource_system/resource_system.h"
#include "../resource_system/resources/mesh.h"
#include "../resource_system/resources/texture.h" // TODO: DELETE THIS ASAP
#include "../resource_system/resources/cubemap.h" // TODO: DELETE THIS ASAP

#include "../../components/camera_component.h"
#include "../../components/drawable_component.h"
#include "../../components/directional_light_component.h"
#include "../../components/point_light_component.h"

namespace_begin

Renderer::Renderer()
	: m_window(nullptr)
	, m_renderer(nullptr)
	, m_profiler(nullptr)
	, m_context()
	, shaderStorage(nullptr)
	, geometryPass(nullptr)
	, shadowMappingPass(nullptr)
	, lighting(nullptr)
	, environment(nullptr)
	, postProcessor(nullptr)
	, screenSampler(nullptr)
	, m_viewport(glm::vec2(0, 0))
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(Config& config, ResourceSystem& resSystem)
{
	bool ret = true;

	Json::Value root = ReadJsonFromFile("../data/configs/" + config.GetGrahicsConfigPath());
	m_viewport.x = root["resolution"][0].asInt();
	m_viewport.y = root["resolution"][1].asInt();
	bool presentationModeEnabled = root["presentation mode"]["enabled"].asBool();
	float aspectRatio = root["presentation mode"]["aspect ratio"][0].asFloat()
		/ root["presentation mode"]["aspect ratio"][1].asFloat();
	int screenOffset = root["presentation mode"]["screen offset"].asInt();

	SDL_GL_LoadLibrary(NULL);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	Uint32 flags = SDL_WINDOW_OPENGL;
	if (presentationModeEnabled)
	{
		m_viewport.x = displayMode.w;
		m_viewport.y = displayMode.h;
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	else
	{
		m_viewport.x = std::min(displayMode.w, static_cast<int>(displayMode.h * aspectRatio)) - screenOffset; // TODO: Check this...
		m_viewport.y = displayMode.h - screenOffset;
	}

	if (SDL_CreateWindowAndRenderer(m_viewport.x, m_viewport.y, flags, &m_window, &m_renderer) != 0 || !m_window || !m_renderer)
	{
		LOG(0, "Could not initialize SDL opengl");
		ret = false;
	}

	m_context = SDL_GL_CreateContext(m_window);
	if (!m_context)
	{
		LOG(0, "Error initializing context");
		ret = false;
	}

	SDL_GL_SetSwapInterval(0);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		LOG(0, "Error initializing GLAD");
		ret = false;
	}

	m_profiler = std::make_unique<GPUProfiler>();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	m_profiler->CreateQueries();
	shaderStorage = std::make_unique<ShadersStorage>();
	shaderStorage->ParseAndStore("../data/shaders/");

	geometryPass = std::make_unique<GeometryPass>(resSystem, *this);
	shadowMappingPass = std::make_unique<ShadowMappingPass>(resSystem, *this);
	lighting = std::make_unique<Lighting>(resSystem, *this);
	environment = std::make_unique<EnvironmentPass>(resSystem, *this);
	postProcessor = std::make_unique<PostProcessor>(resSystem, *this);
	screenSampler = std::make_unique<ScreenSampler>(resSystem, *this);

	glGenBuffers(1, &m_cameraBlockID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBlockID);
	glBufferData(GL_UNIFORM_BUFFER, 4 * 64, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraBlockID);

	constexpr Mesh::Vertex QuadStrip[]{ {-1.f, 1.f, 0.f, 0.f, 1.f}, {-1.f, -1.f, 0.f, 0.f, 0.f}, {1.f, 1.f, 0.f, 1.f, 1.f}, {1.f, -1.f, 0.f, 1.f, 0.f} };
	constexpr Mesh::Vertex CubeVert[]{ {-1.f, 1.f, -1.f}, {-1.f, -1.f, -1.f}, {1.f, -1.f, -1.f}, { 1.f, -1.f, -1.f}, {1.f, 1.f, -1.f}, {-1.f, 1.f, -1.f}, {-1.f, -1.f, 1.f}, {-1.f, -1.f, -1.f}, {-1.f, 1.f, -1.f}, {-1.f, 1.f, -1.f}, {-1.f, 1.f, 1.f}, {-1.f, -1.f, 1.f}, {1.f, -1.f, -1.f}, {1.f, -1.f, 1.f}, { 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, {1.f, 1.f, -1.f}, {1.f, -1.f, -1.f}, {-1.f, -1.f, 1.f}, {-1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, {1.f, -1.f, 1.f}, {-1.f, -1.f, 1.f}, {-1.f, 1.f, -1.f}, {1.f, 1.f, -1.f}, {1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, {-1.f, 1.f, 1.f}, {-1.f, 1.f, -1.f}, {-1.f, -1.f, -1.f}, {-1.f, -1.f, 1.f}, {1.f, -1.f, -1.f}, {1.f, -1.f, -1.f}, {-1.f, -1.f, 1.f}, {1.f, -1.f, 1.f} };

	quad = resSystem.Create<Mesh>();
	quad->AttachVertices(QuadStrip, SizeofArray(QuadStrip));
	quad->SetLayout(Mesh::Layout::tri_strip);
	quad->Load();

	cube = resSystem.Create<Mesh>();
	cube->AttachVertices(CubeVert, SizeofArray(CubeVert));
	cube->SetLayout(Mesh::Layout::vert_array);
	cube->Load();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetCurrentContext();
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 130");

	SDL_SetWindowTitle(m_window, config.GetWinName().c_str());

	{
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		environment->SetCaptureMatrices(captureProjection, captureViews);
	}

	return ret;
}

void Renderer::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();
}

void Renderer::PostUpdate()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_MakeCurrent(m_window, m_context);
	SDL_GL_SwapWindow(m_window);
}

void Renderer::Update(ResourceSystem& resSystem)
{
	UpdateCameraBlock();
	Render();
}

void Renderer::Shutdown()
{
	quad->Free();
	cube->Free();

	postProcessor->Destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
}

void Renderer::Render()
{
	int geometryProfileID = m_profiler->StartQuery("Geometry");
	GeometrySource geometrySource;
	geometrySource.Drawables = m_drawables;
	geometryPass->Render(*this, geometrySource);
	m_profiler->EndQuery(geometryProfileID);

	// SSAO is needed on lighting pass, so we need to compute it here even if its more postprocessing than anything else
	postProcessor->ComputeSSAO(*this, geometryPass->GetNormals(), geometryPass->GetDepth(), geometryPass->GetMaterial());

	int shadowsDepthID = m_profiler->StartQuery("ShadowsDepth");
	ShadowMappingPassSource shadowPassSource;
	shadowPassSource.Drawables = m_drawables;
	shadowPassSource.DirectionalLights = m_directionalLights;
	shadowPassSource.PointLights = m_pointLights;
	shadowMappingPass->Render(*this, shadowPassSource);
	m_profiler->EndQuery(shadowsDepthID);

	int pbrProfileID = m_profiler->StartQuery("PBR");
	LightingSource lightingSource;
	lightingSource.Albedo = geometryPass->GetAlbedo();
	lightingSource.Normals = geometryPass->GetNormals();
	lightingSource.Material = geometryPass->GetMaterial();
	lightingSource.Depth = geometryPass->GetDepth();
	lightingSource.IrradianceMap = environment->m_irradianceCubemap;
	lightingSource.DirectionalLights = m_directionalLights;
	lightingSource.PointLights = m_pointLights;
	lighting->Render(*this, lightingSource);
	m_profiler->EndQuery(pbrProfileID);

	int environmentProfileID = m_profiler->StartQuery("Environment");
	environment->Render(*this);
	m_profiler->EndQuery(environmentProfileID);

	int postProcessorProfileID = m_profiler->StartQuery("PostProcessor");
	PostProcessorSource postProcessorSource;
	postProcessorSource.SceneSample = lighting->GetSceneHDR();
	postProcessorSource.BloomSample = lighting->GetBloomHDR();
	postProcessorSource.Depth = geometryPass->GetDepth();
	postProcessor->Render(*this, postProcessorSource);
	m_profiler->EndQuery(postProcessorProfileID);

	int sampleToScreenProfileID = m_profiler->StartQuery("Sample to screen");
	ScreenSamplerSource screenSamplerSource;
	screenSamplerSource.SceneSample[(int)ScreenSamplerSource::OutputSample::scene] = postProcessorSource.OutputSample;
	screenSamplerSource.SceneSample[(int)ScreenSamplerSource::OutputSample::depth] = geometryPass->GetDepth();
	screenSamplerSource.SceneSample[(int)ScreenSamplerSource::OutputSample::ssao] = geometryPass->GetMaterial();
	screenSampler->Render(*this, screenSamplerSource);
	m_profiler->EndQuery(sampleToScreenProfileID);
	m_profiler->SleepQueries();
}

void Renderer::AddDrawable(std::shared_ptr<DrawableComponent> drawable)
{
	m_drawables.emplace_back(std::move(drawable));
}

void Renderer::RemoveDrawable(std::shared_ptr<DrawableComponent> drawable)
{
	m_drawables.erase(std::remove_if(m_drawables.begin(), m_drawables.end(), [drawable](auto sharedPtr)
		{
			return sharedPtr == drawable;
		}), m_drawables.end());
}

void Renderer::AddLight(std::shared_ptr<DirectionalLightComponent> directionalLight)
{
	m_directionalLights.emplace_back(directionalLight);
}

void Renderer::RemoveLight(std::shared_ptr<DirectionalLightComponent> directionalLight)
{
	m_directionalLights.erase(std::remove_if(m_directionalLights.begin(), m_directionalLights.end(), [directionalLight](auto sharedPtr)
		{
			return sharedPtr == directionalLight;
		}), m_directionalLights.end());
}

void Renderer::AddLight(std::shared_ptr<PointLightComponent> pointLight)
{
	m_pointLights.emplace_back(pointLight);
}

void Renderer::RemoveLight(std::shared_ptr<PointLightComponent> pointLight)
{
	m_pointLights.erase(std::remove_if(m_pointLights.begin(), m_pointLights.end(), [pointLight](auto sharedPtr)
		{
			return sharedPtr == pointLight;
		}), m_pointLights.end());
}

void Renderer::SetCamera(std::shared_ptr<CameraComponent> camera)
{
	m_camera = camera;
	m_camera->SetAspectRatio(static_cast<float>(m_viewport.x) / static_cast<float>(m_viewport.y));
}

void Renderer::SetViewport(const glm::vec2& viewport)
{
	glViewport(0, 0, viewport.x, viewport.y);
	m_viewport = viewport;
}

void Renderer::ImguiProcessEvent(SDL_Event& SDLEvent)
{
	ImGui_ImplSDL2_ProcessEvent(&SDLEvent);
}

void Renderer::HandleEvent(SDL_Event& SDLEvent)
{
	switch (SDLEvent.window.event)
	{
	case SDL_WINDOWEVENT_RESIZED:
		SetViewport(glm::vec2(SDLEvent.window.data1, SDLEvent.window.data2));
		break;
	case SDL_WINDOWEVENT_CLOSE:
		Engine::Get()->Quit();
		break;
	}
}

void Renderer::UpdateCameraBlock() const
{
	const glm::mat4x4& proj = m_camera->GetProjection();
	const glm::mat4x4& view = m_camera->GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, m_cameraBlockID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4 * 16, (const float*)glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, 4 * 16, 4 * 16, (const float*)glm::value_ptr(proj));
	glBufferSubData(GL_UNIFORM_BUFFER, 4 * 32, 4 * 16, (const float*)glm::value_ptr(glm::inverse(view)));
	glBufferSubData(GL_UNIFORM_BUFFER, 4 * 48, 4 * 16, (const float*)glm::value_ptr(glm::inverse(proj)));
}

namespace_end
