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

#include "lighting.h"
#include "../render_sources/lighting_source.h"
#include "../render_passes/geometry_pass.h"
#include "../render_passes/environment_pass.h"

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
#include "../../../components/transform_component.h"
#include "../../../components/camera_component.h"
#include "../../../components/directional_light_component.h"
#include "../../../components/point_light_component.h"

namespace_begin

Lighting::Lighting(ResourceSystem& resSystem, const Renderer& renderer)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_dirLightProgram = resSystem.Create<Program>();
	m_pointLightProgram = resSystem.Create<Program>();
	m_finalShadingProgram = resSystem.Create<Program>();

	m_dirLightProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_dirLightProgram->AttachFragmentObject(renderer.shaderStorage->GetDirectionalShadingFrag().c_str());
	m_dirLightProgram->AttachFragmentObject(renderer.shaderStorage->GetUtils().c_str());
	m_dirLightProgram->AttachFragmentObject(renderer.shaderStorage->GetPBRutils().c_str());
	m_dirLightProgram->CompileProgram();

	m_pointLightProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_pointLightProgram->AttachFragmentObject(renderer.shaderStorage->GetPointShadingFrag().c_str());
	m_pointLightProgram->AttachFragmentObject(renderer.shaderStorage->GetUtils().c_str());
	m_pointLightProgram->AttachFragmentObject(renderer.shaderStorage->GetPBRutils().c_str());
	m_pointLightProgram->CompileProgram();

	m_finalShadingProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_finalShadingProgram->AttachFragmentObject(renderer.shaderStorage->GetFinalShadingFrag().c_str());
	m_finalShadingProgram->AttachFragmentObject(renderer.shaderStorage->GetUtils().c_str());
	m_finalShadingProgram->AttachFragmentObject(renderer.shaderStorage->GetPBRutils().c_str());
	m_finalShadingProgram->CompileProgram();

	m_dirLightProgram->Bind();
	m_dirLightProgram->SetUniformTexture("gBuffer.albedo", 0);
	m_dirLightProgram->SetUniformTexture("gBuffer.normal", 1);
	m_dirLightProgram->SetUniformTexture("gBuffer.material", 2);
	m_dirLightProgram->SetUniformTexture("gBuffer.depth", 3);
	m_dirLightProgram->SetUniformTexture("cumHDRsample", 4);
	m_dirLightProgram->SetUniformTexture("light.depth1", 5);
	m_dirLightProgram->SetUniformTexture("light.depth2", 6);
	m_dirLightProgram->SetUniformTexture("light.depth3", 7);

	m_pointLightProgram->Bind();
	m_pointLightProgram->SetUniformTexture("gBuffer.albedo", 0);
	m_pointLightProgram->SetUniformTexture("gBuffer.normal", 1);
	m_pointLightProgram->SetUniformTexture("gBuffer.material", 2);
	m_pointLightProgram->SetUniformTexture("gBuffer.depth", 3);
	m_pointLightProgram->SetUniformTexture("cumHDRsample", 4);
	m_pointLightProgram->SetUniformTexture("light.depth", 5);

	m_finalShadingProgram->Bind();
	m_finalShadingProgram->SetUniformTexture("HDRsample", 0);
	m_finalShadingProgram->SetUniformTexture("gBuffer.albedo", 1);
	m_finalShadingProgram->SetUniformTexture("gBuffer.normal", 2);
	m_finalShadingProgram->SetUniformTexture("gBuffer.material", 3);
	m_finalShadingProgram->SetUniformTexture("gBuffer.depth", 4);
	m_finalShadingProgram->SetUniformTexture("irradianceMap", 5);
	m_finalShadingProgram->SetUniformTexture("prefilterMap", 6);
	m_finalShadingProgram->SetUniformTexture("brdfLUT", 7);

	m_bloomTextureHDR = resSystem.Create<Texture>();
	m_bloomTextureHDR->Create();
	m_bloomTextureHDR->Bind(0);
	m_bloomTextureHDR->DefineParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_bloomTextureHDR->DefineParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_bloomTextureHDR->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_bloomTextureHDR->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_bloomTextureHDR->DefineParameter(GL_TEXTURE_BASE_LEVEL, 0);
	m_bloomTextureHDR->DefineParameter(GL_TEXTURE_MAX_LEVEL, 4); // viewport, viewport/2, viewport/4, viewport/8, viewport/16
	m_bloomTextureHDR->DefineBuffer(viewport, 0, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_bloomTextureHDR->DefineBuffer(viewport / glm::vec2(2.f, 2.f), 1, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_bloomTextureHDR->DefineBuffer(viewport / glm::vec2(4.f, 4.f), 2, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_bloomTextureHDR->DefineBuffer(viewport / glm::vec2(8.f, 8.f), 3, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_bloomTextureHDR->DefineBuffer(viewport / glm::vec2(16.f, 16.f), 4, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);

	m_lightAccumulationPP.CreateBuffers(resSystem);
	m_lightAccumulationPP.DefineBuffersParameters(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_lightAccumulationPP.DefineBuffersParameters(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_lightAccumulationPP.DefineBuffersParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_lightAccumulationPP.DefineBuffersParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_lightAccumulationPP.DefineBuffers(viewport, 0, GL_RGB16F, GL_RGB, GL_FLOAT);
	m_lightAccumulationPP.Create(resSystem);
	m_lightAccumulationPP.AttachExtraBuffer(renderer.geometryPass->GetDepth(), GL_DEPTH_ATTACHMENT, 0);
}

Lighting::~Lighting()
{
	m_bloomTextureHDR->Free();
	m_lightAccumulationPP.Free();
}

void Lighting::Destroy()
{
	m_dirLightProgram->Free();
	m_pointLightProgram->Free();
	m_finalShadingProgram->Free();
	m_bloomTextureHDR->Free();
	m_lightAccumulationPP.Free();
}

void Lighting::Render(const Renderer& renderer, const LightingSource& source)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_lightAccumulationPP.BindFBO();
	m_lightAccumulationPP.ClearAttachments(GL_COLOR_BUFFER_BIT);
	m_lightAccumulationPP.ResetState();

	//----LightPass---DirLights----//
	glDisable(GL_DEPTH_TEST);
	m_dirLightProgram->Bind();
	source.Albedo->Bind(0);
	source.Normals->Bind(1);
	source.Material->Bind(2);
	source.Depth->Bind(3);
	glm::vec3 cameraPos = renderer.GetCamera()->GetPos();
	m_dirLightProgram->SetUniformVec3("camPos", cameraPos.x, cameraPos.y, cameraPos.z);
	m_dirLightProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
	for (std::shared_ptr<DirectionalLightComponent> light : source.DirectionalLights)
	{
		m_lightAccumulationPP.GetFrontBuffer()->Bind(4);

		glm::vec3 dir, color;
		dir = light->GetDirection();
		color = light->GetColor();
		m_dirLightProgram->SetUniformVec3("light.dir", dir.x, dir.y, dir.z);
		m_dirLightProgram->SetUniformVec3("light.color", color.r, color.g, color.b);
		m_dirLightProgram->SetUniformInt("light.castShadows", light->IsCastingShadows());
		glm::mat4x4 lightMatrices[3];
		light->GetLightSpaceCascadesProjViewMatrix(lightMatrices);
		m_dirLightProgram->SetUniformMat4("light.lightSpaceMatrix[0]", false, (const float*)glm::value_ptr(lightMatrices[0]));
		m_dirLightProgram->SetUniformMat4("light.lightSpaceMatrix[1]", false, (const float*)glm::value_ptr(lightMatrices[1]));
		m_dirLightProgram->SetUniformMat4("light.lightSpaceMatrix[2]", false, (const float*)glm::value_ptr(lightMatrices[2]));
		if (light->IsCastingShadows())
		{
			Texture* shadowMaps[3];
			light->GetShadowMapArray(shadowMaps);
			shadowMaps[0]->Bind(5);
			shadowMaps[1]->Bind(6);
			shadowMaps[2]->Bind(7);
		}


		renderer.quad->BindAndDraw();
		m_lightAccumulationPP.SwapBuffers(0);
	}

	//----LightPass---PointLights----//
	m_pointLightProgram->Bind();
	source.Albedo->Bind(0);
	source.Normals->Bind(1);
	source.Material->Bind(2);
	source.Depth->Bind(3);
	m_pointLightProgram->SetUniformVec3("camPos", cameraPos.x, cameraPos.y, cameraPos.z);
	m_pointLightProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
	for (std::shared_ptr<PointLightComponent> light : source.PointLights)
	{
		m_lightAccumulationPP.GetFrontBuffer()->Bind(4);
		std::shared_ptr<TransformComponent> transform = light->GetOwner()->GetComponent<TransformComponent>();
		glm::vec3 lightPos = transform->GetPosition();
		glm::vec3 lightColor = light->GetColor();
		m_pointLightProgram->SetUniformVec3("light.pos", lightPos.x, lightPos.y, lightPos.z);
		m_pointLightProgram->SetUniformVec3("light.color", lightColor.r, lightColor.g, lightColor.b);
		m_pointLightProgram->SetUniformFloat("light.radius", light->GetRadius());
		m_pointLightProgram->SetUniformInt("light.castShadows", light->IsCastingShadows());
		if (light->IsCastingShadows())
			light->GetShadowMap()->Bind(5);

		renderer.quad->BindAndDraw();
		m_lightAccumulationPP.SwapBuffers(0);
	}

	//----DeferredPass----//

	FrameBufferObject* pingPongFBO = m_lightAccumulationPP.GetFBO();
	const Texture* availableToDrawPPTexture = m_lightAccumulationPP.GetBackBuffer();
	pingPongFBO->AttachTarget(availableToDrawPPTexture, GL_COLOR_ATTACHMENT0, 0);
	pingPongFBO->AttachTarget(m_bloomTextureHDR, GL_COLOR_ATTACHMENT1, 0);
	uint32 attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	pingPongFBO->DefineDrawAttachments(attachments, SizeofArray(attachments));

	m_finalShadingProgram->Bind();
	m_lightAccumulationPP.GetFrontBuffer()->Bind(0);
	source.Albedo->Bind(1);
	source.Normals->Bind(2);
	source.Material->Bind(3);
	source.Depth->Bind(4);
	source.IrradianceMap->Bind(5);
	renderer.environment->m_prefilterCubemap->Bind(6);
	renderer.environment->m_brdfLUTTexture->Bind(7);
	m_finalShadingProgram->SetUniformVec3("camPos", cameraPos.x, cameraPos.y, cameraPos.z);
	m_finalShadingProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
	renderer.quad->BindAndDraw();
	glEnable(GL_DEPTH_TEST);
}

namespace_end