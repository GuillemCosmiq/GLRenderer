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

#include <random>

namespace_begin

PostProcessor::PostProcessor(ResourceSystem& resSystem, const Renderer& renderer)
	: filtersFlags(0xffff)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_gaussianBlurProgram = resSystem.Create<Program>();
	m_colorCorrectionProgram = resSystem.Create<Program>();
	m_objectMotionBlur = resSystem.Create<Program>();
	m_cameraMotionBlurProgram = resSystem.Create<Program>();
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

	m_objectMotionBlur->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_objectMotionBlur->AttachFragmentObject(renderer.shaderStorage->GetObjectMotionBlurfrag().c_str());
	m_objectMotionBlur->CompileProgram();
	m_objectMotionBlur->Bind();
	m_objectMotionBlur->SetUniformTexture("sceneSample", 0);
	m_objectMotionBlur->SetUniformTexture("gBuffer.velocity", 1);

	m_cameraMotionBlurProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_cameraMotionBlurProgram->AttachFragmentObject(renderer.shaderStorage->GetCameraMotionBlurfrag().c_str());
	m_cameraMotionBlurProgram->AttachFragmentObject(renderer.shaderStorage->GetUtils().c_str());
	m_cameraMotionBlurProgram->CompileProgram();
	m_cameraMotionBlurProgram->Bind();
	m_cameraMotionBlurProgram->SetUniformTexture("sceneSample", 0);
	m_cameraMotionBlurProgram->SetUniformTexture("gBuffer.albedo", 1);
	m_cameraMotionBlurProgram->SetUniformTexture("gBuffer.depth", 2);

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

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	glm::vec3 ssaoKernel[64];
	for (int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		float scale = float(i) / 64.0;
		scale = 0.1f + (scale * scale) * (1.0f - 0.1f); // we use a lerp to add more samples close to the center and less to the perimeter.
		sample *= scale;
		ssaoKernel[i] = sample;
	}

	glm::vec3 ssaoNoise[16];
	for (int i = 0; i < 16; ++i)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise[i] = noise;
	}

	m_ssaoProgram = resSystem.Create<Program>();
	m_ssaoProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_ssaoProgram->AttachFragmentObject(renderer.shaderStorage->GetSSAOFrag().c_str());
	m_ssaoProgram->AttachFragmentObject(renderer.shaderStorage->GetUtils().c_str());
	m_ssaoProgram->CompileProgram();
	m_ssaoProgram->Bind();
	m_ssaoProgram->SetUniformTexture("noiseTexture", 0);
	m_ssaoProgram->SetUniformTexture("gBuffer.normals", 1);
	m_ssaoProgram->SetUniformTexture("gBuffer.depth", 2);
	m_ssaoProgram->SetUniformVec3Array("samples", glm::value_ptr(ssaoKernel[0]), SizeofArray(ssaoKernel));

	m_noiseTexture = resSystem.Create<Texture>();
	m_noiseTexture->Create();
	m_noiseTexture->Bind(0);
	m_noiseTexture->DefineParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_noiseTexture->DefineParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	m_noiseTexture->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_noiseTexture->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_noiseTexture->DefineBuffer(glm::vec2(4.f, 4.f), 0, GL_RGB16F, GL_RGB, GL_FLOAT, ssaoNoise);

	m_ssaoTexture = resSystem.Create<Texture>();
	m_ssaoTexture->Create();
	m_ssaoTexture->Bind(0);
	m_ssaoTexture->DefineParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_ssaoTexture->DefineParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	m_ssaoTexture->DefineParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_ssaoTexture->DefineParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_ssaoTexture->DefineBuffer(renderer.GetViewport(), 0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	m_ssaoBlurProgram = resSystem.Create<Program>();
	m_ssaoBlurProgram->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_ssaoBlurProgram->AttachFragmentObject(renderer.shaderStorage->GetSSAOBlurFrag().c_str());
	m_ssaoBlurProgram->CompileProgram();
	m_ssaoBlurProgram->Bind();
	m_ssaoBlurProgram->SetUniformTexture("ssaoTexture", 0);

	m_ssaoFbo = resSystem.Create<FrameBufferObject>();
	m_ssaoFbo->Init();
	m_ssaoFbo->Bind();
	attachment = GL_COLOR_ATTACHMENT0;
	m_ssaoFbo->DefineDrawAttachments(&attachment, 1);
	m_ssaoFbo->DefineReadAttachment(GL_NONE);
}

PostProcessor::~PostProcessor() {}

void PostProcessor::Destroy()
{
	m_gaussianBlurInnerStepTexture->Free();

	m_pingPong.Free();
	m_gaussianBlurFbo->Free();

	m_gaussianBlurProgram->Free();
	m_colorCorrectionProgram->Free();
	m_objectMotionBlur->Free();
	m_cameraMotionBlurProgram->Free();
	m_fxaaProgram->Free();
	m_vignetteProgram->Free();

	m_noiseTexture->Free();
	m_ssaoTexture->Free();
	m_ssaoBlurProgram->Free();
	m_ssaoProgram->Free();
	m_ssaoFbo->Free();
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
		m_colorCorrectionProgram->SetUniformInt("IsToneMappingActive", filtersFlags & FiltersFlags::ToneMapping);
		m_colorCorrectionProgram->SetUniformInt("IsGammaCorrectionActive", filtersFlags & FiltersFlags::GammaCorrection);
		m_colorCorrectionProgram->SetUniformFloat("gamma", colorCorrectionData.gammaValue);
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

	if (filtersFlags & FiltersFlags::ObjectBlur)
	{
		m_objectMotionBlur->Bind();
		source.OutputSample->Bind(0);
		source.VelocitySample->Bind(1);
		m_objectMotionBlur->SetUniformVec2("viewport", viewport.x, viewport.y);
		renderer.quad->BindAndDraw();
		m_pingPong.SwapBuffers(0);
		source.OutputSample = m_pingPong.GetFrontBuffer();
	}

	//if (filtersFlags & FiltersFlags::MotionBlur)
	//{
	//	m_cameraMotionBlurProgram->Bind();
	//	source.OutputSample->Bind(0);
	//	source.AlbedoSample->Bind(1);
	//	source.Depth->Bind(2);
	//	m_cameraMotionBlurProgram->SetUniformMat4("prevProjViewMatrix", false, (const float*)glm::value_ptr(renderer.GetPrevFrameProjViewMatrix()));
	//	m_cameraMotionBlurProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
	//	renderer.quad->BindAndDraw();
	//	m_pingPong.SwapBuffers(0);
	//	source.OutputSample = m_pingPong.GetFrontBuffer();
	//}

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

void PostProcessor::ComputeSSAO(const Renderer& renderer, const Texture* normals, const Texture* depth, const Texture* output)
{
	glm::vec2 viewport = renderer.GetViewport();

	m_ssaoFbo->Bind();
	m_ssaoFbo->AttachTarget(m_ssaoTexture, GL_COLOR_ATTACHMENT0, 0);
	m_ssaoProgram->Bind();
	m_ssaoProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
	m_ssaoProgram->SetUniformFloat("power", filtersFlags & FiltersFlags::SSAO ? ssaoData.power : 0);
	m_noiseTexture->Bind(0);
	normals->Bind(1);
	depth->Bind(2);
	renderer.quad->BindAndDraw();

	glColorMask(false, false, true, false);
	m_ssaoFbo->AttachTarget(output, GL_COLOR_ATTACHMENT0, 0);
	m_ssaoBlurProgram->Bind();
	m_ssaoBlurProgram->SetUniformVec2("viewport", viewport.x, viewport.y);
	m_ssaoTexture->Bind(0);
	renderer.quad->BindAndDraw();
	glColorMask(true, true, true, true);
}

namespace_end