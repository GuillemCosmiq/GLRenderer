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

#include "point_light_component.h"

#include "../engine.h"
#include "../entity.h"
#include "../systems/renderer/renderer.h"

namespace_begin

PointLightComponent::PointLightComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_radius(1.f)
	, m_color(1.f, 1.f, 1.f)
	, m_castShadows(true)
{
}

void PointLightComponent::AddedToScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->renderer->AddLight(std::static_pointer_cast<PointLightComponent>(shared_from_this()));
}

void PointLightComponent::RemovedFromScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->renderer->RemoveLight(std::static_pointer_cast<PointLightComponent>(shared_from_this()));
}

void PointLightComponent::SetRadius(float radius)
{
	m_radius = radius;
}

void PointLightComponent::SetColor(const glm::vec3& color)
{
	m_color = color;
}

void PointLightComponent::SetShadowMap(Cubemap* shadowMap)
{
	m_shadowMap = shadowMap;
}

void PointLightComponent::SetShadowCasting(bool enable)
{
	m_castShadows = enable;
}

void PointLightComponent::GetLightSpaceTransformationMatrices(std::vector<glm::mat4>& matrices, const glm::vec3& position)
{
	float aspect = (float)1024 / (float)1024;
	float fNear = 1.0f;
	float fFar = m_radius;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, fNear, fFar);

	matrices.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	matrices.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	matrices.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	matrices.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	matrices.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	matrices.push_back(shadowProj *
		glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
}

namespace_end