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

	m_pingPong.CreateBuffers(resSystem, viewport, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_pingPong.DefineBuffersParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_pingPong.Create(resSystem);

	m_gaussianBlurPP.CreateBuffers(resSystem, viewport, GL_RGB16F, GL_RGB, GL_FLOAT);
	m_gaussianBlurPP.DefineBuffersParameters(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	m_gaussianBlurPP.DefineBuffersParameters(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	m_gaussianBlurPP.DefineBuffersParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_gaussianBlurPP.DefineBuffersParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_gaussianBlurPP.Create(resSystem);

	// TODO: Improve bloom by using mipmaps and downscaling the image at each step. Check Jesus presentation at Drive, but basically we can set max mipamap numbers using texparameters and
	// even define them with texImage2D :)
}

PostProcessor::~PostProcessor() {}

void PostProcessor::Destroy()
{
	m_pingPong.Free();
	m_gaussianBlurPP.Free();

	m_gaussianBlurProgram->Free();
	m_colorCorrectionProgram->Free();
	m_fxaaProgram->Free();
	m_vignetteProgram->Free();
}

void PostProcessor::Render(const Renderer& renderer, PostProcessorSource& source)
{
	glm::vec2 viewport = renderer.GetViewport();

	source.OutputSample = source.SceneSample;

	const Texture* filteredBloomSample;
	if (filtersFlags & FiltersFlags::Bloom)
	{
		filteredBloomSample = ComputeGaussianBlur(renderer, source.BloomSample, viewport, bloomData.iterations);
	}
	else
	{
		filteredBloomSample = source.BloomSample;
	}

	m_pingPong.BindFBO();
	m_pingPong.ResetState();

	if (filtersFlags & FiltersFlags::ColorCorrection)
	{
		m_colorCorrectionProgram->Bind();
		source.SceneSample->Bind(0);
		filteredBloomSample->Bind(1);
		m_colorCorrectionProgram->SetUniformFloat("exposure", colorCorrectionData.exposure);
		m_colorCorrectionProgram->SetUniformInt("IsToneMappingActive", colorCorrectionData.toneMapping);
		m_colorCorrectionProgram->SetUniformInt("IsGammaCorrectionActive", colorCorrectionData.gammaCorrection);
		m_colorCorrectionProgram->SetUniformFloat("gamma", colorCorrectionData.gamma);
		m_colorCorrectionProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
		renderer.quad->BindAndDraw();
		m_pingPong.SwapBuffers();
		source.OutputSample = m_pingPong.GetFrontBuffer();
	}
	
	if (filtersFlags & FiltersFlags::FXAA)
	{
		m_fxaaProgram->Bind();
		source.OutputSample->Bind(0);
		m_fxaaProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
		renderer.quad->BindAndDraw();
		m_pingPong.SwapBuffers();
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
		m_pingPong.SwapBuffers();
		source.OutputSample = m_pingPong.GetFrontBuffer();
	}
}

const Texture* PostProcessor::ComputeGaussianBlur(const Renderer& renderer, const Texture* sample, const glm::vec2& sampleSize, int numSamples)
{
	assert(sample != nullptr && numSamples > 0);
	m_gaussianBlurProgram->Bind();
	m_gaussianBlurProgram->SetUniformVec2("viewport", sampleSize.x, sampleSize.y);

	m_gaussianBlurPP.BindFBO();
	m_gaussianBlurPP.ResetState();
	sample->Bind(0);
	for (unsigned int i = 0; i < numSamples; ++i)
	{
		m_gaussianBlurProgram->SetUniformInt("horizontal", !(m_gaussianBlurPP.GetIndex() % 2));
		renderer.quad->BindAndDraw();
		m_gaussianBlurPP.SwapBuffers();
		m_gaussianBlurPP.GetFrontBuffer()->Bind(0);
	}

	return m_gaussianBlurPP.GetFrontBuffer();
}

namespace_end