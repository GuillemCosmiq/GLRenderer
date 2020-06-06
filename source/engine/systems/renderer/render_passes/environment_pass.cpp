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

#include "environment_pass.h"

#include "lighting.h"
#include "../renderer.h"
#include "../shaders_storage.h"
#include "../../resource_system/resource_system.h"
#include "../../resource_system/resources/render_target.h"
#include "../../resource_system/resources/program.h"
#include "../../resource_system/resources/mesh.h"
#include "../../resource_system/resources/render_buffer.h"
#include "../../resource_system/resources/texture.h"
#include "../../resource_system/resources/cubemap.h"

namespace_begin

EnvironmentPass::EnvironmentPass(ResourceSystem& resSystem, const Renderer& renderer)
{
	needComputeEnvironmentMaps = false;

	m_fbo = resSystem.Create<FrameBufferObject>();

	m_skyboxProgram = resSystem.Create<Program>();
	m_skyboxProgram->AttachVertexObject(renderer.shaderStorage->GetEnvironmentVert().c_str());
	m_skyboxProgram->AttachFragmentObject(renderer.shaderStorage->GetEnvironmentFrag().c_str());
	m_skyboxProgram->CompileProgram();
	m_skyboxProgram->Bind();
	m_skyboxProgram->SetUniformTexture("environmentMap", 0);

	m_equirectangularToCubemapProgram = resSystem.Create<Program>();
	m_equirectangularToCubemapProgram->AttachVertexObject(renderer.shaderStorage->GetEquirectangularToCubemapVert().c_str());
	m_equirectangularToCubemapProgram->AttachFragmentObject(renderer.shaderStorage->GetEquirectangularToCubemapFrag().c_str());
	m_equirectangularToCubemapProgram->CompileProgram();
	m_equirectangularToCubemapProgram->Bind();
	m_equirectangularToCubemapProgram->SetUniformTexture("environmentMap", 0);

	m_irradianceConvolutionProgram = resSystem.Create<Program>();
	m_irradianceConvolutionProgram->AttachVertexObject(renderer.shaderStorage->GetEquirectangularToCubemapVert().c_str());
	m_irradianceConvolutionProgram->AttachFragmentObject(renderer.shaderStorage->GetIrradianceConvolutionFrag().c_str());
	m_irradianceConvolutionProgram->CompileProgram();
	m_irradianceConvolutionProgram->Bind();
	m_irradianceConvolutionProgram->SetUniformTexture("environmentMap", 0);

	m_prefilterEnvProgram = resSystem.Create<Program>();
	m_prefilterEnvProgram->AttachVertexObject(renderer.shaderStorage->GetEquirectangularToCubemapVert().c_str());
	m_prefilterEnvProgram->AttachFragmentObject(renderer.shaderStorage->GetPrefilterEnvironmentFrag().c_str());
	m_prefilterEnvProgram->CompileProgram();
	m_prefilterEnvProgram->Bind();
	m_prefilterEnvProgram->SetUniformTexture("environmentMap", 0);

	m_brdfProgram = resSystem.Create<Program>();
	m_brdfProgram->AttachVertexObject(renderer.shaderStorage->GetBrdfVert().c_str());
	m_brdfProgram->AttachFragmentObject(renderer.shaderStorage->GetBrdfFrag().c_str());
	m_brdfProgram->CompileProgram();
	m_brdfProgram->Bind();

	m_environmentCubemap = resSystem.Create<Cubemap>();
	m_environmentCubemap->Create();
	m_environmentCubemap->Bind(0);
	m_environmentCubemap->DefineParameters(GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
	m_environmentCubemap->DefineBuffer(glm::vec2(1024, 1024), GL_RGB16F, GL_RGB, GL_FLOAT, NULL);

	m_irradianceCubemap = resSystem.Create<Cubemap>();
	m_irradianceCubemap->Create();
	m_irradianceCubemap->Bind(0);
	m_irradianceCubemap->DefineParameters(GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
	m_irradianceCubemap->DefineBuffer(glm::vec2(32, 32), GL_RGB16F, GL_RGB, GL_FLOAT, NULL);

	m_prefilterCubemap = resSystem.Create<Cubemap>();
	m_prefilterCubemap->Create();
	m_prefilterCubemap->Bind(0);
	m_prefilterCubemap->DefineParameters(GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	m_prefilterCubemap->DefineBuffer(glm::vec2(128, 128), GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_prefilterCubemap->GenerateMipMaps();

	m_brdfLUTTexture = resSystem.Create<Texture>();
	m_brdfLUTTexture->Create();
	m_brdfLUTTexture->Bind(0);
	m_brdfLUTTexture->DefineParameters(GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
	m_brdfLUTTexture->DefineBuffer({ 512, 512 }, GL_RG16F, GL_RG, GL_FLOAT, NULL);
}

EnvironmentPass::~EnvironmentPass() {}

void EnvironmentPass::Destroy()
{
	m_skyboxProgram->Free();
	m_equirectangularToCubemapProgram->Free();
	m_irradianceConvolutionProgram->Free();
	m_prefilterEnvProgram->Free();
	m_brdfProgram->Free();
	m_equirectangularEnv->Free();
	m_environmentCubemap->Free();
	m_irradianceCubemap->Free();
	m_prefilterCubemap->Free();
	m_brdfLUTTexture->Free();
}

void EnvironmentPass::Render(const Renderer& renderer)
{
	if (needComputeEnvironmentMaps)
	{
		m_fbo->Init();
		uint32 attachment = GL_COLOR_ATTACHMENT0;
		m_fbo->DefineDrawAttachments(&attachment, 0);
		m_fbo->Bind();
		ComputeCubemapFromEquirectangularMap(renderer);
		ComputeIrradianceMap(renderer);
		ComputePrefilterMap(renderer);
		ComputeBRDFMap(renderer);
		m_fbo->Free();
		needComputeEnvironmentMaps = false;
	}

	renderer.lighting->GetHDRFbo()->Bind();
	glDepthFunc(GL_LEQUAL);
	m_skyboxProgram->Bind();
	m_environmentCubemap->Bind(0);
	renderer.cube->BindAndDraw();
	glDepthFunc(GL_LESS);
}

void EnvironmentPass::ComputeCubemapFromEquirectangularMap(const Renderer& renderer)
{
	m_equirectangularToCubemapProgram->Bind();
	m_equirectangularEnv->Bind(0);
	m_equirectangularToCubemapProgram->SetUniformMat4("proj", false, (const float*)glm::value_ptr(m_captureProjection));
	glViewport(0, 0, 1024, 1024);
	for (int i = 0; i < 6; ++i)
	{
		m_equirectangularToCubemapProgram->SetUniformMat4("view", false, (const float*)glm::value_ptr(m_captureViews[i]));
		m_fbo->AttachTarget(m_environmentCubemap, GL_COLOR_ATTACHMENT0, i, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.cube->BindAndDraw();
	}
}

void EnvironmentPass::ComputeIrradianceMap(const Renderer& renderer)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_irradianceConvolutionProgram->Bind();
	m_environmentCubemap->Bind(0);
	m_irradianceConvolutionProgram->SetUniformMat4("proj", false, (const float*)glm::value_ptr(m_captureProjection));
	glViewport(0, 0, 32, 32);
	for (int i = 0; i < 6; ++i)
	{
		m_irradianceConvolutionProgram->SetUniformMat4("view", false, (const float*)glm::value_ptr(m_captureViews[i]));
		m_fbo->AttachTarget(m_irradianceCubemap, GL_COLOR_ATTACHMENT0, i, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.cube->BindAndDraw();
	}
}

void EnvironmentPass::ComputePrefilterMap(const Renderer& renderer)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_prefilterEnvProgram->Bind();
	m_prefilterEnvProgram->SetUniformMat4("proj", false, (const float*)glm::value_ptr(m_captureProjection));
	m_environmentCubemap->Bind(0);

	int maxMipLevels = 5;
	for (int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glViewport(0, 0, mipWidth, mipHeight); // lvl 2 is 32x32

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		m_prefilterEnvProgram->SetUniformFloat("roughness", roughness);
		for (int i = 0; i < 6; ++i)
		{
			m_prefilterEnvProgram->SetUniformMat4("view", false, (const float*)glm::value_ptr(m_captureViews[i]));
			m_fbo->AttachTarget(m_prefilterCubemap, GL_COLOR_ATTACHMENT0, i, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderer.cube->BindAndDraw();
		}
	}
	glViewport(0, 0, viewport.x, viewport.y);
}

void EnvironmentPass::ComputeBRDFMap(const Renderer& renderer)
{
	glm::vec2 viewport = renderer.GetViewport();
	m_fbo->Bind();
	m_fbo->AttachTarget(m_brdfLUTTexture, GL_COLOR_ATTACHMENT0);
	glViewport(0, 0, 512, 512);
	m_brdfProgram->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderer.quad->BindAndDraw();
	glViewport(0, 0, viewport.x, viewport.y);
}

void EnvironmentPass::SetCaptureMatrices(glm::mat4& projection, glm::mat4 (&captureViews)[6])
{
	m_captureProjection = projection;
	memcpy(m_captureViews, captureViews, sizeof(captureViews));
}

void EnvironmentPass::SetEquirectangularEnv(Texture* equirectangularMap)
{
	m_equirectangularEnv = equirectangularMap;
	needComputeEnvironmentMaps = true;
}

namespace_end