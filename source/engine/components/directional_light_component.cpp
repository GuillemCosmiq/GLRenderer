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

#include "camera_component.h"
#include "../engine.h"
#include "../entity.h"
#include "../systems/renderer/renderer.h"
#include "../systems/resource_system/resources/texture_array.h"

namespace_begin

DirectionalLightComponent::DirectionalLightComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_cameraEmitterID(0)
	, m_enabled(false)
	, m_direction(0.f, -1.f, 0.f)
	, m_color(1.f, 1.f, 1.f)
{
	m_shadowData.CastShadows = false;
	m_shadowData.ShadowMaps[0] = nullptr;
	m_shadowData.ShadowMaps[1] = nullptr;
	m_shadowData.ShadowMaps[2] = nullptr;
	m_shadowData.LightSpaceCascadesMatrices[0] = glm::mat4x4(1.f);
	m_shadowData.LightSpaceCascadesMatrices[1] = glm::mat4x4(1.f);
	m_shadowData.LightSpaceCascadesMatrices[2] = glm::mat4x4(1.f);
	m_shadowData.FrustumSplits[0] = 0.f;
	m_shadowData.FrustumSplits[1] = 0.f;
	m_shadowData.FrustumSplits[2] = 0.f;
}

void DirectionalLightComponent::AddedToScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->renderer->AddLight(std::static_pointer_cast<DirectionalLightComponent>(shared_from_this()));
	std::shared_ptr<CameraComponent> sceneCamera = Engine::Get()->renderer->GetCamera();

	m_cameraEmitterID = sceneCamera->cameraDirtyEmitter.AddReceptor([this](CameraComponent* cameraCmp)
		{
			if (m_shadowData.CastShadows)
			{
				std::vector<glm::vec3> worldSpaceCameraFurstumCorners;
				cameraCmp->GetWorldSpaceFrustumCorners(worldSpaceCameraFurstumCorners);
				ComputeOrtoProjViewContainingOBB(worldSpaceCameraFurstumCorners, cameraCmp->GetNearPlane(), 0.f, cameraCmp->GetFarPlane());
			}
		});
}

void DirectionalLightComponent::RemovedFromScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->renderer->RemoveLight(std::static_pointer_cast<DirectionalLightComponent>(shared_from_this()));
	Engine::Get()->renderer->GetCamera()->cameraDirtyEmitter.RemoveListener(m_cameraEmitterID);
}

void DirectionalLightComponent::Enable(bool value)
{
	m_enabled = value;
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
	m_shadowData.CastShadows = enable;

	// we need to calculate lightMatrices when enabling csm or otherwise we would need to wait until camera updates
	std::shared_ptr<CameraComponent> sceneCamera = Engine::Get()->renderer->GetCamera();
	std::vector<glm::vec3> worldSpaceCameraFurstumCorners;
	sceneCamera->GetWorldSpaceFrustumCorners(worldSpaceCameraFurstumCorners);
	ComputeOrtoProjViewContainingOBB(worldSpaceCameraFurstumCorners, sceneCamera->GetNearPlane(), 0.f, sceneCamera->GetFarPlane());
}

void DirectionalLightComponent::SetShadowMap(Texture* shadowMaps[3])
{
	m_shadowData.ShadowMaps[0] = shadowMaps[0];
	m_shadowData.ShadowMaps[1] = shadowMaps[1];
	m_shadowData.ShadowMaps[2] = shadowMaps[2];
}

void DirectionalLightComponent::SetFrustumSplits(float frustumSplits[3])
{
	m_shadowData.FrustumSplits[0] = frustumSplits[0];
	m_shadowData.FrustumSplits[1] = frustumSplits[1];
	m_shadowData.FrustumSplits[2] = frustumSplits[2];
}

bool DirectionalLightComponent::IsEnabled() const
{
	return m_enabled;
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
	return m_shadowData.CastShadows;
}

const TextureArray* DirectionalLightComponent::GetShadowMap() const
{
	return nullptr;
}

void DirectionalLightComponent::GetFrustumSplits(float frustumSplits[3]) const
{
	frustumSplits[0] = m_shadowData.FrustumSplits[0];
	frustumSplits[1] = m_shadowData.FrustumSplits[1];
	frustumSplits[2] = m_shadowData.FrustumSplits[2];
}

void DirectionalLightComponent::GetShadowMapArray(Texture* shadowMaps[3]) const
{
	shadowMaps[0] = m_shadowData.ShadowMaps[0];
	shadowMaps[1] = m_shadowData.ShadowMaps[1];
	shadowMaps[2] = m_shadowData.ShadowMaps[2];
}

void DirectionalLightComponent::ComputeOrtoProjViewContainingOBB(const std::vector<glm::vec3>& corners, float nearClip, float nearClipOffset, float farClip)
{
	#define NUM_CASCADES 3
	assert(m_shadowData.FrustumSplits[0] != 0.f && m_shadowData.FrustumSplits[1] != 0.f && m_shadowData.FrustumSplits[2] != 0.f);

	for (int cascadeIterator = 0; cascadeIterator < NUM_CASCADES; ++cascadeIterator)
	{
		float prevSplitDistance = cascadeIterator == 0 ? nearClip : m_shadowData.FrustumSplits[cascadeIterator - 1];
		float splitDistance = m_shadowData.FrustumSplits[cascadeIterator];

		std::vector<glm::vec3> boundedCorners = corners;
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
		glm::mat4x4 viewMatrix = glm::lookAt(tmpLightPos, frustumCentroid, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 cascadeExtents = maxExtents - minExtents;
		glm::mat4x4 ortoProjMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);

		// The rounding matrix that ensures that shadow edges do not shimmer
		glm::mat4 shadowMatrix = ortoProjMatrix * viewMatrix;
		glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		shadowOrigin = shadowMatrix * shadowOrigin;
		float mShadowMapSize = static_cast<float>(1024);
		shadowOrigin = shadowOrigin * mShadowMapSize / 2.0f;

		glm::vec4 roundedOrigin = glm::round(shadowOrigin);
		glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
		roundOffset = roundOffset * 2.0f / mShadowMapSize;
		roundOffset.z = 0.0f;
		roundOffset.w = 0.0f;

		glm::mat4 shadowProj = ortoProjMatrix;
		shadowProj[3] += roundOffset;
		ortoProjMatrix = shadowProj;
		m_shadowData.LightSpaceCascadesMatrices[cascadeIterator] = shadowProj * viewMatrix;
	}
}

void DirectionalLightComponent::GetLightSpaceCascadesProjViewMatrix(glm::mat4x4 matrices[3]) const
{
	matrices[0] = m_shadowData.LightSpaceCascadesMatrices[0];
	matrices[1] = m_shadowData.LightSpaceCascadesMatrices[1];
	matrices[2] = m_shadowData.LightSpaceCascadesMatrices[2];
}

namespace_end