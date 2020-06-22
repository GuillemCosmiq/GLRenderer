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

#include "scene.h"

#include "engine.h"
#include "entity.h"
#include "components/camera_component.h"
#include "components/transform_component.h"
#include "components/autorotation_component.h"
#include "components/automovement_component.h"
#include "components/drawable_component.h"
#include "components/directional_light_component.h"
#include "components/point_light_component.h"
#include "systems/resource_system/resource_system.h"
#include "systems/resource_system/resources/texture.h"
#include "systems/resource_system/resources/cubemap.h"
#include "systems/renderer/renderer.h"
#include "systems/renderer/render_passes/environment_pass.h"
#include "../utilities/model_loader.h"

namespace_begin

// TODO: Entity creation and resource loading should be splitted.
/* To keep it simple for the scope of the project the scene will load an obj and that's it.
   The ideal is to be able to load predefined json/bin scenes or coded scenes using inheritance. */

Scene::Scene()
	: m_name("")
{
}

Scene::~Scene()
{
}

void Scene::Initialize(Config& config, ResourceSystem& resSystem)
{
	// TODO: Unload environment maps. add shutdown method :)
	// TODO: Organize this better and unload everything. Also, component camera is super hackish in terms of transformation ;)

	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	static_cast<void>(player->AddComponent<CameraComponent>());
	AddEntity(player);

	stbi_set_flip_vertically_on_load(true);
	ModelLoader::SceneObjects scene;

	ModelLoader::Load("../data/3d_scenes/planeCobbleStone.obj", scene, resSystem);
	for (std::shared_ptr<Entity> entity : scene)
	{
		std::shared_ptr<GLEngine::TransformComponent> transformCmp = entity->GetComponent<GLEngine::TransformComponent>();
		glm::mat4 transform = transformCmp->GetMatrix();
		transform = glm::translate(transform, glm::vec3(0, -20, 0));
		transform = glm::scale(transform, glm::vec3(100, 100, 100));
		transformCmp->SetMatrix(transform);

		std::shared_ptr<GLEngine::DrawableComponent> drawableCmp = entity->GetComponent<GLEngine::DrawableComponent>();
		drawableCmp->SetNumTiles(10);
		AddEntity(entity);
	}
	scene.clear();

	//-------------------------------------------------//
	LoadAndStoreSwitchableObjScenes(resSystem, "Mitsuba Rusted Iron", "../data/3d_scenes/export3dcoatPBR_rustediron.obj");
	LoadAndStoreSwitchableObjScenes(resSystem, "Mitsuba Gold", "../data/3d_scenes/export3dcoatPBR_gold.obj");
	LoadAndStoreSwitchableObjScenes(resSystem, "Cerberus Revolver", "../data/3d_scenes/Cerberus.obj", glm::vec3(10.f, 10.f, 10.f));

	m_currentSwitchableObjScene = "Mitsuba Rusted Iron";
	AddEntity(m_loadedSwitchableObjScenes[m_currentSwitchableObjScene]);

	LoadAndStoreEnvironment(resSystem, "Apartment", "../data/cubemaps/Alexs_Apt_2k.hdr");
	LoadAndStoreEnvironment(resSystem, "Pine Tree", "../data/cubemaps/Arches_E_PineTree_3k.hdr");
	LoadAndStoreEnvironment(resSystem, "Ueno Shrine", "../data/cubemaps/03-Ueno-Shrine_3k.hdr");
	SetCurrentEnvironmentFromLoadedMap("Apartment");

	//------------------------------------------------------------//

	//std::shared_ptr<Entity> mainLight = std::make_shared<Entity>();
	//std::shared_ptr<TransformComponent> transform = mainLight->AddComponent<TransformComponent>();
	//std::shared_ptr<DirectionalLightComponent> lightCmp = mainLight->AddComponent<DirectionalLightComponent>();
	//lightCmp->SetColor(glm::vec3(1.0f, 0.3f, 0.3f));
	//lightCmp->SetDirection(glm::vec3(0.f, -1.f, 0.f));
	////AddEntity(mainLight);

	std::shared_ptr<Entity> mainLight2 = std::make_shared<Entity>();
	std::shared_ptr<TransformComponent> transform2 = mainLight2->AddComponent<TransformComponent>();
	std::shared_ptr<DirectionalLightComponent> lightCmp2 = mainLight2->AddComponent<DirectionalLightComponent>();
	lightCmp2->SetColor(glm::vec3(10.0f, 0.3f, 0.f));
	lightCmp2->SetDirection(glm::vec3(0.f, -0.8f, -0.2f));
	
	Texture* shadowMaps[3];
	for (int i = 0; i < 3; ++i)
	{
		shadowMaps[i] = resSystem.Create<Texture>();
		shadowMaps[i]->Create();
		shadowMaps[i]->Bind(0);
		shadowMaps[i]->DefineParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		shadowMaps[i]->DefineParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		shadowMaps[i]->DefineParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		shadowMaps[i]->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		shadowMaps[i]->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		shadowMaps[i]->DefineBuffer(glm::vec2(2048, 2048), 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	lightCmp2->SetShadowCasting(true);
	lightCmp2->SetShadowMap(shadowMaps);
	AddEntity(mainLight2);


//	lightCmp->SetColor(glm::vec3(1000.0f, 0.3f, 0.3f));
//	lightCmp->SetDirection(glm::vec3(0.f, -1.f, 0.f));
	//AddEntity(mainLight);

	//lightCmp->SetColor(glm::vec3(0.0f, 0.3f, 1.0f));
	//lightCmp->SetDirection(glm::vec3(0.2f, 0.2f, -0.3f));
//	AddEntity(mainLight);

	//std::shared_ptr<Entity> pointLight = std::make_shared<Entity>();
	//std::shared_ptr<TransformComponent> transform = pointLight->AddComponent<TransformComponent>();
	//glm::mat4 pointMatrix = transform->GetMatrix();
	//pointMatrix = glm::translate(pointMatrix, glm::vec3(5.f, 15.f, 0.f));
	//transform->SetMatrix(pointMatrix);
	//std::shared_ptr<PointLightComponent> pointLightCmp = pointLight->AddComponent<PointLightComponent>();
	//Cubemap* pointShadowMap = resSystem.Create<Cubemap>();
	//pointShadowMap->Create();
	//pointShadowMap->Bind(0);
	//pointShadowMap->DefineParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//pointShadowMap->DefineParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//pointShadowMap->DefineParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//pointShadowMap->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//pointShadowMap->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//pointShadowMap->DefineBuffer(glm::vec2(1024, 1024), GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//pointLightCmp->SetColor(glm::vec3(100.f, 0.3f, 0.3f));
	//pointLightCmp->SetRadius(120.f);
	//pointLightCmp->SetShadowCasting(true);
	//pointLightCmp->SetShadowMap(pointShadowMap);
	//AddEntity(pointLight);
}

void Scene::Update()
{
	for (std::shared_ptr<Entity> entity : m_entities)
		entity->Update();
}

void Scene::PostUpdate()
{
	for (std::shared_ptr<Entity> entity : m_entities)
		entity->PostUpdate();
}

void Scene::AddEntity(std::shared_ptr<Entity> entity)
{
	assert(std::find(m_entities.begin(), m_entities.end(), entity) == m_entities.end());
	entity->AddedToScene(shared_from_this());
	m_entities.emplace_back(entity);
}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity)
{
	assert(std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end());
	std::shared_ptr<Entity> validEntity = entity;
	validEntity->RemovedFromScene(shared_from_this());
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [validEntity](auto sharedPtr)
		{
			return sharedPtr == validEntity;
		}), m_entities.end());
}

void Scene::SetCurrentSwitchableObjScene(const std::string& objScene)
{
	RemoveEntity(m_loadedSwitchableObjScenes[m_currentSwitchableObjScene]);
	AddEntity(m_loadedSwitchableObjScenes[objScene]);
	m_currentSwitchableObjScene = objScene;
}

const std::map<const std::string, std::shared_ptr<Entity>>& Scene::GetLoadedSwitchableObjScenes() const
{
	return m_loadedSwitchableObjScenes;
}

void Scene::LoadAndStoreSwitchableObjScenes(ResourceSystem& resSystem, const std::string& name, const std::string& path, const glm::vec3& scale)
{
	ModelLoader::SceneObjects objModels;
	ModelLoader::Load(path.c_str(), objModels, resSystem);
	for (std::shared_ptr<Entity> model : objModels)
	{
		model->AddComponent<AutorotationComponent>();
		model->AddComponent<AutomovementComponent>();
		if (scale != glm::vec3(1.f, 1.f, 1.f))
		{
			std::shared_ptr transformCmp = model->GetComponent<TransformComponent>();
			glm::mat4x4 modelMatrix = transformCmp->GetMatrix();
			transformCmp->SetMatrix(glm::scale(modelMatrix, scale));
		}
		std::shared_ptr drawableCmp = model->GetComponent<DrawableComponent>();
		drawableCmp->SetIfMaskedForMotionBlur(true);
		m_loadedSwitchableObjScenes.insert(std::make_pair(name, model));
	}
	objModels.clear();
}

void Scene::SetRotationStrenghOfObjScenes(float value)
{
	for (auto& model : m_loadedSwitchableObjScenes)
	{
		std::shared_ptr autorotationCmp = model.second->GetComponent<AutorotationComponent>();
		autorotationCmp->SetRotation(value);
	}
}

void Scene::SetMovementParametersOfObjScenes(float distance, float strengh)
{
	for (auto& model : m_loadedSwitchableObjScenes)
	{
		std::shared_ptr automovementCmp = model.second->GetComponent<AutomovementComponent>();
		automovementCmp->SetAmplitude(distance);
		automovementCmp->SetStrengh(strengh);
	}
}

void Scene::SetCurrentEnvironmentFromLoadedMap(const std::string& name)
{
	Engine::Get()->renderer->environment->SetEquirectangularEnv(m_loadedEnvironmentMaps[name]);
}

const std::map<const std::string, Texture*>& Scene::GetLoadedEnvironments() const
{
	return m_loadedEnvironmentMaps;
}

void Scene::LoadAndStoreEnvironment(ResourceSystem& resSystem, const std::string& name, const std::string& path)
{
	Texture* environmentTexture;
	int envWidth, envHeight, nrComponents;
	float* data = stbi_loadf(path.c_str(), &envWidth, &envHeight, &nrComponents, 0);
	if (data)
	{
		environmentTexture = resSystem.Create<Texture>();

		environmentTexture->Create();
		environmentTexture->Bind(0);
		environmentTexture->DefineParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		environmentTexture->DefineParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		environmentTexture->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		environmentTexture->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (nrComponents == 3)
			environmentTexture->DefineBuffer({ envWidth, envHeight }, 0, GL_RGB16F, GL_RGB, GL_FLOAT, data);
		else
			environmentTexture->DefineBuffer({ envWidth, envHeight }, 0, GL_RGBA16F, GL_RGBA, GL_FLOAT, data);

		m_loadedEnvironmentMaps.insert(std::make_pair(name, environmentTexture));
		stbi_image_free(data);
	}
}

namespace_end
