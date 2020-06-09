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

#include "postprocessor.h"
#include "../render_sources/postprocessor_source.h"

#include "../renderer.h"
#include "../shaders_storage.h"
#include "../../resource_system/resource_system.h"
#include "../../resource_system/resources/framebuffer_object.h"
#include "../../resource_system/resources/program.h"
#include "../../resource_system/resources/mesh.h"
#include "../../resource_system/resources/texture.h"

namespace_begin

PostProcessor::PostProcessor(ResourceSystem& resSystem, const Renderer& renderer)
	: filtersFlags(0xff)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_gaussianBlurProgram = resSystem.Create<Program>();
	m_colorCorrectionProgram = resSystem.Create<Program>();
	m_fxaaProgram = resSystem.Create<Program>();
	m_vignetteProgram = resSystem.Create<Program>();

	m_gaussianBlurProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_gaussianBlurProgram->AttachFragmentObject(renderer.shaderStorage->GetBlurFrag().c_str());
	m_gaussianBlurProgram->CompileProgram();
	m_gaussianBlurProgram->Bind();
	m_gaussianBlurProgram->SetUniformTexture("ping", 0);

	m_colorCorrectionProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_colorCorrectionProgram->AttachFragmentObject(renderer.shaderStorage->GetColorCorrectionFrag().c_str());
	m_colorCorrectionProgram->CompileProgram();
	m_colorCorrectionProgram->Bind();
	m_colorCorrectionProgram->SetUniformTexture("sceneSampleHDR", 0);
	m_colorCorrectionProgram->SetUniformTexture("filteredBloomSampleHDR", 1);

	m_fxaaProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_fxaaProgram->AttachFragmentObject(renderer.shaderStorage->GetFXAAfrag().c_str());
	m_fxaaProgram->CompileProgram();
	m_fxaaProgram->Bind();
	m_fxaaProgram->SetUniformTexture("sceneSample", 0);

	m_vignetteProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_vignetteProgram->AttachFragmentObject(renderer.shaderStorage->GetVignetteFrag().c_str());
	m_vignetteProgram->CompileProgram();
	m_vignetteProgram->Bind();
	m_vignetteProgram->SetUniformTexture("sceneSample", 0);

	m_pingPong.CreateBuffers(resSystem);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_pingPong.DefineBuffers(viewport, 0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	m_pingPong.Create(resSystem);

	m_gaussianBlurInnerStepTexture = resSystem.Create<Texture>();
	m_gaussianBlurInnerStepTexture->Create();
	m_gaussianBlurInnerStepTexture->Bind(0);
	m_gaussianBlurInnerStepTexture->DefineParameter(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	m_gaussianBlurInnerStepTexture->DefineParameter(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	m_gaussianBlurInnerStepTexture->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_gaussianBlurInnerStepTexture->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gaussianBlurInnerStepTexture->DefineParameter(GL_TEXTURE_BASE_LEVEL, 0);
	m_gaussianBlurInnerStepTexture->DefineParameter(GL_TEXTURE_MAX_LEVEL, 4); // viewport, viewport/2, viewport/4, viewport/8, viewport/16
	m_gaussianBlurInnerStepTexture->DefineBuffer(viewport, 0, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_gaussianBlurInnerStepTexture->DefineBuffer(viewport / glm::vec2(2.f, 2.f), 1, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_gaussianBlurInnerStepTexture->DefineBuffer(viewport / glm::vec2(4.f, 4.f), 2, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_gaussianBlurInnerStepTexture->DefineBuffer(viewport / glm::vec2(8.f, 8.f), 3, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_gaussianBlurInnerStepTexture->DefineBuffer(viewport / glm::vec2(16.f, 16.f), 4, GL_RGB16F, GL_RGB, GL_FLOAT, NULL);
	m_gaussianBlurInnerStepTexture->GenerateMipMaps();

	m_gaussianBlurFbo = resSystem.Create<FrameBufferObject>();
	m_gaussianBlurFbo->Init();
	m_gaussianBlurFbo->Bind();
	uint32 attachment = GL_COLOR_ATTACHMENT0;
	m_gaussianBlurFbo->DefineDrawAttachments(&attachment, 1);
	m_gaussianBlurFbo->DefineReadAttachment(GL_NONE);
}

PostProcessor::~PostProcessor() {}

void PostProcessor::Destroy()
{
	m_gaussianBlurInnerStepTexture->Free();

	m_pingPong.Free();
	m_gaussianBlurFbo->Free();

	m_gaussianBlurProgram->Free();
	m_colorCorrectionProgram->Free();
	m_fxaaProgram->Free();
	m_vignetteProgram->Free();
}

void PostProcessor::Render(const Renderer& renderer, PostProcessorSource& source)
{
	glm::vec2 viewport = renderer.GetViewport();

	source.OutputSample = source.SceneSample;
	int bloomActive = 0;
	if (filtersFlags & FiltersFlags::Bloom)
	{
		bloomActive = 1;
		source.BloomSample->Bind(0);
		source.BloomSample->GenerateMipMaps();
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, source.BloomSample, m_gaussianBlurInnerStepTexture, viewport, 0, glm::vec2(1.f, 0.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, source.BloomSample, m_gaussianBlurInnerStepTexture, viewport / 2.f, 1, glm::vec2(1.f, 0.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, source.BloomSample, m_gaussianBlurInnerStepTexture, viewport / 4.f, 2, glm::vec2(1.f, 0.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, source.BloomSample, m_gaussianBlurInnerStepTexture, viewport / 8.f, 3, glm::vec2(1.f, 0.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, source.BloomSample, m_gaussianBlurInnerStepTexture, viewport / 16.f, 4, glm::vec2(1.f, 0.f));

		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, m_gaussianBlurInnerStepTexture, source.BloomSample, viewport, 0, glm::vec2(0.f, 1.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, m_gaussianBlurInnerStepTexture, source.BloomSample, viewport / 2.f, 1, glm::vec2(0.f, 1.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, m_gaussianBlurInnerStepTexture, source.BloomSample, viewport / 4.f, 2, glm::vec2(0.f, 1.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, m_gaussianBlurInnerStepTexture, source.BloomSample, viewport / 8.f, 3, glm::vec2(0.f, 1.f));
		ComputeTwoPassGaussianBlur(renderer, m_gaussianBlurFbo, m_gaussianBlurInnerStepTexture, source.BloomSample, viewport / 16.f, 4, glm::vec2(0.f, 1.f));
		glViewport(0, 0, viewport.x, viewport.y);
	}

	m_pingPong.BindFBO();
	m_pingPong.ResetState();

	if (filtersFlags & FiltersFlags::ColorCorrection)
	{
		m_colorCorrectionProgram->Bind();
		source.SceneSample->Bind(0);
		source.BloomSample->Bind(1);
		m_colorCorrectionProgram->SetUniformFloat("exposure", colorCorrectionData.exposure);
		m_colorCorrectionProgram->SetUniformInt("IsBloomActive", bloomActive);
		m_colorCorrectionProgram->SetUniformFloat("BloomLODIntesities[0]", bloomData.LODIntesities[0]);
		m_colorCorrectionProgram->SetUniformFloat("BloomLODIntesities[1]", bloomData.LODIntesities[1]);
		m_colorCorrectionProgram->SetUniformFloat("BloomLODIntesities[2]", bloomData.LODIntesities[2]);
		m_colorCorrectionProgram->SetUniformFloat("BloomLODIntesities[3]", bloomData.LODIntesities[3]);
		m_colorCorrectionProgram->SetUniformFloat("BloomLODIntesities[4]", bloomData.LODIntesities[4]);
		m_colorCorrectionProgram->SetUniformInt("IsToneMappingActive", colorCorrectionData.toneMapping);
		m_colorCorrectionProgram->SetUniformInt("IsGammaCorrectionActive", colorCorrectionData.gammaCorrection);
		m_colorCorrectionProgram->SetUniformFloat("gamma", colorCorrectionData.gamma);
		m_colorCorrectionProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
		renderer.quad->BindAndDraw();
		m_pingPong.SwapBuffers(0);
		source.OutputSample = m_pingPong.GetFrontBuffer();
	}
	
	if (filtersFlags & FiltersFlags::FXAA)
	{
		m_fxaaProgram->Bind();
		source.OutputSample->Bind(0);
		m_fxaaProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
		renderer.quad->BindAndDraw();
		m_pingPong.SwapBuffers(0);
		source.OutputSample = m_pingPong.GetFrontBuffer();
	}

	if (filtersFlags & FiltersFlags::Vignette)
	{
		m_vignetteProgram->Bind();
		source.OutputSample->Bind(0);
		m_vignetteProgram->SetUniformFloat("radius", vignetteData.radius);
		m_vignetteProgram->SetUniformFloat("softness", vignetteData.softness);
		m_vignetteProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
		renderer.quad->BindAndDraw();
		m_pingPong.SwapBuffers(0);
		source.OutputSample = m_pingPong.GetFrontBuffer();
	}
}

void PostProcessor::ComputeTwoPassGaussianBlur(const Renderer& renderer, const FrameBufferObject* fbo, const Texture* input, const Texture* output, glm::vec2& resolution, int LOD, glm::vec2& direction)
{
	fbo->Bind();
	fbo->AttachTarget(output, GL_COLOR_ATTACHMENT0, LOD);
	input->Bind(0);
	m_gaussianBlurProgram->Bind();
	m_gaussianBlurProgram->SetUniformInt("LOD", LOD);
	glm::vec2 normalizedDirection = glm::normalize(direction);
	m_gaussianBlurProgram->SetUniformVec2("direction", normalizedDirection.x, normalizedDirection.y);
	m_gaussianBlurProgram->SetUniformVec2("viewport", resolution.x, resolution.y);
	glViewport(0, 0, resolution.x, resolution.y);
	renderer.quad->BindAndDraw();
}

namespace_end