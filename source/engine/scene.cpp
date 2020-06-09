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
	// TODO: Organize this better and unload everything. Also, component camera is super hackish in terms of transformation ;)

	stbi_set_flip_vertically_on_load(true);
	ModelLoader::SceneObjects scene;

	//-------------------------------------------------//

	ModelLoader::Load("../data/3d_scenes/export3dcoatPBR_gold.obj", scene, resSystem);
	for (std::shared_ptr<Entity> entity : scene)
	{
		entity->AddComponent<AutorotationComponent>();
		AddEntity(entity);
	}
	scene.clear();
	
//	ModelLoader::Load("../data/3d_scenes/planeCobbleStone.obj", scene, resSystem);
	for (std::shared_ptr<Entity> entity : scene)
		AddEntity(entity);
	scene.clear();

	//------------------------------------------------------------//

	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	static_cast<void>(player->AddComponent<CameraComponent>());
	AddEntity(player);

	{
		int envWidth, envHeight, nrComponents;
		float* environmentData = stbi_loadf("../data/cubemaps/Alexs_Apt_2k.hdr", &envWidth, &envHeight, &nrComponents, 0);
		if (environmentData)
		{
			Texture* environment = resSystem.Create<Texture>();

			environment->Create();
			environment->Bind(0);
			environment->DefineParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			environment->DefineParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			environment->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			environment->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			if (nrComponents == 3)
				environment->DefineBuffer({ envWidth, envHeight }, 0, GL_RGB16F, GL_RGB, GL_FLOAT, environmentData);
			else
				environment->DefineBuffer({ envWidth, envHeight }, 0, GL_RGBA16F, GL_RGBA, GL_FLOAT, environmentData);
			stbi_image_free(environmentData);
			Engine::Get()->renderer->environment->SetEquirectangularEnv(environment);
		}
	}

	std::shared_ptr<Entity> mainLight = std::make_shared<Entity>();
	std::shared_ptr<TransformComponent> transform = mainLight->AddComponent<TransformComponent>();
	std::shared_ptr<DirectionalLightComponent> lightCmp = mainLight->AddComponent<DirectionalLightComponent>();
	lightCmp->SetColor(glm::vec3(1.0f, 0.3f, 0.3f));
	lightCmp->SetDirection(glm::vec3(0.f, -1.f, 0.f));
	AddEntity(mainLight);

	std::shared_ptr<Entity> mainLight2 = std::make_shared<Entity>();
	std::shared_ptr<TransformComponent> transform2 = mainLight2->AddComponent<TransformComponent>();
	std::shared_ptr<DirectionalLightComponent> lightCmp2 = mainLight2->AddComponent<DirectionalLightComponent>();
	lightCmp2->SetColor(glm::vec3(0.2f, 1.0f, 0.3f));
	lightCmp2->SetDirection(glm::vec3(0.2f, -0.8f, 0.2f));
	AddEntity(mainLight2);


//	lightCmp->SetColor(glm::vec3(1000.0f, 0.3f, 0.3f));
//	lightCmp->SetDirection(glm::vec3(0.f, -1.f, 0.f));
	//AddEntity(mainLight);

	//lightCmp->SetColor(glm::vec3(0.0f, 0.3f, 1.0f));
	//lightCmp->SetDirection(glm::vec3(0.2f, 0.2f, -0.3f));
//	AddEntity(mainLight);

	std::shared_ptr<Entity> pointLight = std::make_shared<Entity>();
	transform = pointLight->AddComponent<TransformComponent>();
	glm::mat4 pointMatrix = transform->GetMatrix();
	pointMatrix = glm::translate(pointMatrix, glm::vec3(0.f, 10.f, 0.f));
	transform->SetMatrix(pointMatrix);
	std::shared_ptr<PointLightComponent> pointLightCmp = pointLight->AddComponent<PointLightComponent>();
	Cubemap* pointShadowMap = resSystem.Create<Cubemap>();
	pointShadowMap->Create();
	pointShadowMap->Bind(0);
	pointShadowMap->DefineParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	pointShadowMap->DefineParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	pointShadowMap->DefineParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	pointShadowMap->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	pointShadowMap->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	pointShadowMap->DefineBuffer(glm::vec2(1024, 1024), GL_DEPTH, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	pointLightCmp->SetColor(glm::vec3(1000.f, 0.3f, 0.3f));
	pointLightCmp->SetRadius(1200.f);
	pointLightCmp->SetShadowCasting(false);
	pointLightCmp->SetShadowMap(pointShadowMap);
	//AddEntity(pointLight);
}

void Scene::Update()
{
	for (std::shared_ptr<Entity> entity : m_entities)
	{
		entity->Update();
	}
}

void Scene::AddEntity(std::shared_ptr<Entity> entity)
{
	assert(std::find(m_entities.begin(), m_entities.end(), entity) == m_entities.end());
	entity->AddedToScene(shared_from_this());
	m_entities.emplace_back(entity);
}

void Scene::RemoveEntity(std::weak_ptr<Entity> entity)
{
	assert(!entity.expired());
	std::shared_ptr<Entity> validEntity = entity.lock();
	validEntity->RemovedFromScene(shared_from_this());
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [validEntity](auto sharedPtr)
		{
			return sharedPtr == validEntity;
		}), m_entities.end());
}

namespace_end
