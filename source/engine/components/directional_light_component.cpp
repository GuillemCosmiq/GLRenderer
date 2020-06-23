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
#include "../systems/resource_system/resources/texture_array.h"

namespace_begin

DirectionalLightComponent::DirectionalLightComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_direction(0.f, -1.f, 0.f)
	, m_color(1.f, 1.f, 1.f)
	, m_castShadows(false)
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

void DirectionalLightComponent::SetShadowMap(Texture* shadowMaps[3])
{
	m_shadowMap[0] = shadowMaps[0];
	m_shadowMap[1] = shadowMaps[1];
	m_shadowMap[2] = shadowMaps[2];
}

const glm::vec3& DirectionalLightComponent::GetDirection() const
{
	return m_direction;
}

const glm::vec3& DirectionalLightComponent::GetColor() const
{
	return m_color;
}

bool DirectionalLightComponent::IsCastingShadows() const
{
	return m_castShadows;
}

const TextureArray* DirectionalLightComponent::GetShadowMap() const
{
	return nullptr;
}

void DirectionalLightComponent::GetShadowMapArray(Texture* shadowMaps[3]) const
{
	shadowMaps[0] = m_shadowMap[0];
	shadowMaps[1] = m_shadowMap[1];
	shadowMaps[2] = m_shadowMap[2];
}

void DirectionalLightComponent::ComputeOrtoProjViewContainingOBB(glm::mat4& outOrtoProj, glm::mat4& outView, const std::vector<glm::vec3>& corners, float nearClip, float nearClipOffset, float farClip)
{
	#define NUM_CASCADES 3
	float cascadeSplits[NUM_CASCADES] = { 0.4f, 0.8f, 1.f };

	for (int cascadeIterator = 0; cascadeIterator < NUM_CASCADES; ++cascadeIterator)
	{
		float prevSplitDistance = cascadeIterator == 0 ? nearClip : cascadeSplits[cascadeIterator - 1];
		float splitDistance = cascadeSplits[cascadeIterator];

		std::vector<glm::vec3> boundedCorners = corners;
		// https://stackoverflow.com/questions/55427438/cascaded-shadow-map-unexpected-behavior
		// here the frustum corners are bounded to the current near far cascade distance.
		for (int i = 0; i < 4; ++i)
		{
			glm::vec3 dist = boundedCorners[i + 4] - boundedCorners[i];
			boundedCorners[i + 4] = boundedCorners[i] + (dist * splitDistance);
			boundedCorners[i] = boundedCorners[i] + (dist * prevSplitDistance);
		}

		glm::vec3 frustumCentroid(0.f);
		for (auto& corner : boundedCorners)
			frustumCentroid += corner;

		frustumCentroid /= 8.f;


		GLfloat radius = 0.0f;
		for (unsigned int i = 0; i < 8; ++i)
		{
			GLfloat distance = glm::length(boundedCorners[i] - frustumCentroid);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		glm::vec3 maxExtents = glm::vec3(radius, radius, radius);
		glm::vec3 minExtents = -maxExtents;

		//Position the viewmatrix looking down the center of the frustum with an arbitrary lighht direction
		glm::vec3 tmpLightPos = frustumCentroid - glm::normalize(m_direction) * -minExtents.z;
		outView = glm::lookAt(tmpLightPos, frustumCentroid, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 cascadeExtents = maxExtents - minExtents;

		outOrtoProj = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);
		m_lightSpaceCascadesMatrices[cascadeIterator] = outOrtoProj * outView;

		// The rounding matrix that ensures that shadow edges do not shimmer
		glm::mat4 shadowMatrix = outOrtoProj * outView;
		glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		shadowOrigin = shadowMatrix * shadowOrigin;
		float mShadowMapSize = static_cast<float>(1024);
		shadowOrigin = shadowOrigin * mShadowMapSize / 2.0f;

		glm::vec4 roundedOrigin = glm::round(shadowOrigin);
		glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
		roundOffset = roundOffset * 2.0f / mShadowMapSize;
		roundOffset.z = 0.0f;
		roundOffset.w = 0.0f;

		glm::mat4 shadowProj = outOrtoProj;
		shadowProj[3] += roundOffset;
		outOrtoProj = shadowProj;
		m_lightSpaceCascadesMatrices[cascadeIterator] = outOrtoProj * outView;

		//float distFromCentroid = splitDistance + nearClipOffset;
		//glm::vec3 tmpLightPos = frustumCentroid - (glm::normalize(m_direction) * distFromCentroid);
		//outView = glm::lookAt(tmpLightPos, frustumCentroid, glm::vec3(0.0f, 1.0f, 0.0f));
		//
		//std::vector<glm::vec3> lightSpaceCorners;
		//lightSpaceCorners.resize(8);
		//for (int i = 0, size = boundedCorners.size(); i < size; ++i)
		//	lightSpaceCorners[i] = outView * glm::vec4(boundedCorners[i], 1.f);
		//
		//glm::vec3 mins = lightSpaceCorners[0];
		//glm::vec3 maxes = lightSpaceCorners[0];
		//for (auto& corner : lightSpaceCorners)
		//{
		//	if (corner.x > maxes.x)
		//		maxes.x = corner.x;
		//	else if (corner.x < mins.x)
		//		mins.x = corner.x;
		//	if (corner.y > maxes.y)
		//		maxes.y = corner.y;
		//	else if (corner.y < mins.y)
		//		mins.y = corner.y;
		//	if (corner.z > maxes.z)
		//		maxes.z = corner.z;
		//	else if (corner.z < mins.z)
		//		mins.z = corner.z;
		//}
		//float distz = maxes.z - mins.z;
		//outOrtoProj = glm::ortho(mins.x, maxes.x, mins.y, maxes.y, -maxes.z, -mins.z);
		//m_lightSpaceCascadesMatrices[cascadeIterator] = outOrtoProj * outView;
	}
}

void DirectionalLightComponent::GetLightSpaceCascadesProjViewMatrix(glm::mat4x4 matrices[3]) const
{
	matrices[0] = m_lightSpaceCascadesMatrices[0];
	matrices[1] = m_lightSpaceCascadesMatrices[1];
	matrices[2] = m_lightSpaceCascadesMatrices[2];
}

namespace_end