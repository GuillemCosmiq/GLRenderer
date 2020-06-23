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

#include "shadow_mapping_pass.h"
#include "../render_sources/shadow_mapping_pass_source.h"

#include "../renderer.h"
#include "../shaders_storage.h"
#include "../../resource_system/resource_system.h"
#include "../../resource_system/resources/framebuffer_object.h"
#include "../../resource_system/resources/program.h"
#include "../../resource_system/resources/mesh.h"
#include "../../resource_system/resources/texture.h"
#include "../../resource_system/resources/texture_array.h"
#include "../../resource_system/resources/cubemap.h"

#include "../../../entity.h"
#include "../../../components/drawable_component.h"
#include "../../../components/transform_component.h"
#include "../../../components/camera_component.h"
#include "../../../components/directional_light_component.h"
#include "../../../components/point_light_component.h"

namespace_begin

ShadowMappingPass::ShadowMappingPass(ResourceSystem& resSystem, const Renderer& renderer)
{
	m_shadowsProgram = resSystem.Create<Program>();
	m_shadowsProgram->AttachVertexObject(renderer.shaderStorage->GetDirShadowsVert().c_str());
	m_shadowsProgram->AttachFragmentObject(renderer.shaderStorage->GetDirShadowsFrag().c_str());
	m_shadowsProgram->CompileProgram();

	m_pointShadowsProgram = resSystem.Create<Program>();
	m_pointShadowsProgram->AttachVertexObject(renderer.shaderStorage->GetPointShadowsVert().c_str());
	m_pointShadowsProgram->AttachGeometryObject(renderer.shaderStorage->GetPointShadowsGeom().c_str());
	m_pointShadowsProgram->AttachFragmentObject(renderer.shaderStorage->GetPointShadowsFrag().c_str());
	m_pointShadowsProgram->CompileProgram();

	m_fbo = resSystem.Create<FrameBufferObject>();
	m_fbo->Init();
	m_fbo->Bind();
	uint32 attachment = GL_NONE;
	m_fbo->DefineDrawAttachments(&attachment, 1);
	m_fbo->DefineReadAttachment(GL_NONE);
}

ShadowMappingPass::~ShadowMappingPass() {}

void ShadowMappingPass::Destroy()
{
	m_shadowsProgram->Free();
	m_pointShadowsProgram->Free();
	m_fbo->Free();
}

void ShadowMappingPass::Render(const Renderer& renderer, const ShadowMappingPassSource& source)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_fbo->Bind();
	glCullFace(GL_FRONT);
	m_shadowsProgram->Bind();
	for (std::shared_ptr<DirectionalLightComponent> light : source.DirectionalLights)
	{
		if (!light->IsCastingShadows())
			continue;
		const TextureArray* shadowMap = light->GetShadowMap();
		Texture* shadowMaps[3];
		light->GetShadowMapArray(shadowMaps);
		const glm::vec2& shadowMapSize = shadowMaps[0]->GetCurrentBufferSize();
		glViewport(0, 0, shadowMapSize.x, shadowMapSize.y);

		float nearClipOffset = 0.f;
		glm::mat4 lightProjection;
		glm::mat4 lightView;
		std::vector<glm::vec3> frustumCorners;
		std::shared_ptr<CameraComponent> camera = renderer.GetCamera();
		camera->GetWorldSpaceFrustumCorners(frustumCorners);
		light->ComputeOrtoProjViewContainingOBB(lightProjection, lightView, frustumCorners, camera->GetNearPlane(), nearClipOffset, camera->GetFarPlane());


		glm::mat4x4 currentCascadelightProjView[3];
		light->GetLightSpaceCascadesProjViewMatrix(currentCascadelightProjView);

		for (int i = 0; i < 3; ++i)
		{
			m_fbo->AttachTarget(shadowMaps[i], GL_DEPTH_ATTACHMENT, 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			//GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			//if (status != GL_FRAMEBUFFER_COMPLETE)
			//{
			//	int a = 0;
			//	a++;
			//	//char* infoLog = (char*)malloc(100);
			//	//glGetProgramInfoLog(programObject, status, NULL, infoLog);
			//	//errormsg = infoLog;
			//	//free(infoLog);
			//}
			//glClear(GL_DEPTH_BUFFER_BIT);
			for (auto& drawable : source.Drawables)
			{
				glm::mat4x4 lightSpaceModelMatrix = currentCascadelightProjView[i] * drawable->GetOwner()->GetComponent<TransformComponent>()->GetMatrix();
				m_shadowsProgram->SetUniformMat4("lightSpaceModelMatrix", false, (const float*)glm::value_ptr(lightSpaceModelMatrix));
				drawable->GetMesh()->BindAndDraw();
			}
		}
	}
	glCullFace(GL_BACK);

	m_pointShadowsProgram->Bind();
	for (std::shared_ptr<PointLightComponent> light : source.PointLights)
	{
		if (!light->IsCastingShadows())
			continue;

		m_fbo->AttachTarget(light->GetShadowMap(), GL_DEPTH_ATTACHMENT, 0);
		const glm::vec2& shadowMapSize = light->GetShadowMap()->GetCurrentBufferSize();
		glViewport(0, 0, shadowMapSize.x, shadowMapSize.y);
		glClear(GL_DEPTH_BUFFER_BIT);

		std::vector<glm::mat4> lightSpaceMatrices;
		std::shared_ptr<TransformComponent> transform = light->GetOwner()->GetComponent<TransformComponent>();
		glm::vec3 lightPos = transform->GetPosition();
		light->GetLightSpaceTransformationMatrices(lightSpaceMatrices, lightPos);

		// TODO: improve uniform matrix sending. ccan be done with a single call
		m_pointShadowsProgram->SetUniformMat4("shadowMatrices[0]", false, (const float*)glm::value_ptr(lightSpaceMatrices[0]));
		m_pointShadowsProgram->SetUniformMat4("shadowMatrices[1]", false, (const float*)glm::value_ptr(lightSpaceMatrices[1]));
		m_pointShadowsProgram->SetUniformMat4("shadowMatrices[2]", false, (const float*)glm::value_ptr(lightSpaceMatrices[2]));
		m_pointShadowsProgram->SetUniformMat4("shadowMatrices[3]", false, (const float*)glm::value_ptr(lightSpaceMatrices[3]));
		m_pointShadowsProgram->SetUniformMat4("shadowMatrices[4]", false, (const float*)glm::value_ptr(lightSpaceMatrices[4]));
		m_pointShadowsProgram->SetUniformMat4("shadowMatrices[5]", false, (const float*)glm::value_ptr(lightSpaceMatrices[5]));
		m_pointShadowsProgram->SetUniformFloat("radius", light->GetRadius());
		m_pointShadowsProgram->SetUniformVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
		for (auto& drawable : source.Drawables)
		{
			m_pointShadowsProgram->SetUniformMat4("model", false, (const float*)glm::value_ptr(drawable->GetOwner()->GetComponent<TransformComponent>()->GetMatrix()));
			drawable->GetMesh()->BindAndDraw();
		}
	}
	glViewport(0, 0, viewport.x, viewport.y);
}

namespace_end