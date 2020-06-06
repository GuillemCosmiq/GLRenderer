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

#include "directional_light_component.h"

#include "../engine.h"
#include "../entity.h"
#include "../systems/renderer/renderer.h"
#include "../systems/resource_system/resources/texture.h"

namespace_begin

DirectionalLightComponent::DirectionalLightComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_direction(0.f, -1.f, 0.f)
	, m_color(1.f, 1.f, 1.f)
	, m_castShadows(false)
	, m_shadowMap(nullptr)
{
}

void DirectionalLightComponent::AddedToScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->renderer->AddLight(std::static_pointer_cast<DirectionalLightComponent>(shared_from_this()));
}

void DirectionalLightComponent::RemovedFromScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->renderer->RemoveLight(std::static_pointer_cast<DirectionalLightComponent>(shared_from_this()));
}

void DirectionalLightComponent::SetDirection(const glm::vec3& direction)
{
	m_direction = direction;
}

void DirectionalLightComponent::SetColor(const glm::vec3& color)
{
	m_color = color;
}

void DirectionalLightComponent::SetShadowCasting(bool enable)
{
	m_castShadows = enable;
}

void DirectionalLightComponent::SetShadowMap(Texture* shadowMap)
{
	m_shadowMap = shadowMap;
}

void DirectionalLightComponent::ComputeOrtoProjViewContainingOBB(glm::mat4& outOrtoProj, glm::mat4& outView, const std::vector<glm::vec3>& corners, float nearClip, float nearClipOffset, float farClip)
{
	glm::vec3 frustumCentroid(0.f);
	for (auto& corner : corners)
	{
		frustumCentroid += corner;
	}
	frustumCentroid /= 8;

	float distFromCentroid = farClip + nearClipOffset;
	glm::vec3 tmpLightPos = frustumCentroid - (m_direction * distFromCentroid);
	outView = glm::lookAt(tmpLightPos, frustumCentroid, glm::vec3(0.0f, 1.0f, 0.0f));

	std::vector<glm::vec3> lightSpaceCorners;
	lightSpaceCorners.resize(8);
	for (int i = 0, size = corners.size(); i < size; ++i)
	{
		lightSpaceCorners[i] = outView * glm::vec4(corners[i], 1.f);
	}
	glm::vec3 mins = lightSpaceCorners[0];
	glm::vec3 maxes = lightSpaceCorners[0];
	for (auto& corner : lightSpaceCorners)
	{
		if (corner.x > maxes.x)
			maxes.x = corner.x;
		else if (corner.x < mins.x)
			mins.x = corner.x;
		if (corner.y > maxes.y)
			maxes.y = corner.y;
		else if (corner.y < mins.y)
			mins.y = corner.y;
		if (corner.z > maxes.z)
			maxes.z = corner.z;
		else if (corner.z < mins.z)
			mins.z = corner.z;
	}
	outOrtoProj = glm::ortho(mins.x, maxes.x, mins.y, maxes.y, -maxes.z, -mins.z);
}

namespace_end