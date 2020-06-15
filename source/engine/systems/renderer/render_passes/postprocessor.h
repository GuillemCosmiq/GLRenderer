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

#ifndef __POST_PROCESSOR_H__
#define __POST_PROCESSOR_H__

#include "../ping_pong.h"
#include "../../resource_system/resources/framebuffer_object.h"
#include "../../resource_system/resources/texture.h"

namespace_begin

class Renderer;
class ResourceSystem;
class PostProcessorSource;
class FrameBufferObject;
class Texture;
class Program;

class PostProcessor
{
public:
	PostProcessor() = delete;
	PostProcessor(ResourceSystem& resSystem, const Renderer& renderer);
	virtual ~PostProcessor();
	
	void Destroy();
	void Render(const Renderer& renderer, PostProcessorSource& postProcessorSource);

	void ComputeTwoPassGaussianBlur(const Renderer& renderer, const FrameBufferObject* fbo, const Texture* input, const Texture* output, glm::vec2& resolution, int LOD, glm::vec2& direction);
	void ComputeSSAO(const Renderer& renderer, const Texture* normals, const Texture* depth, const Texture* output);

public:
	enum FiltersFlags
	{
		Bloom = 1 << 1,
		ColorCorrection = 1 << 2,
		GammaCorrection = 1 << 3,
		ToneMapping = 1 << 4,
		SSAO = 1 << 5,
		FXAA = 1 << 6,
		MotionBlur = 1 << 7,
		Vignette = 1 << 8,
	};
	
	uint16 filtersFlags;

	struct BloomData
	{
		float LODIntesities[5] = { 0.1f, 0.15f, 0.2f, 0.4f, 0.6f };
	} bloomData;

	struct ColorCorrectionData
	{
		float exposure = 1.f;
		float gammaValue = 2.2f;
	} colorCorrectionData;

	struct SSAOData
	{
		float power = 10.f;
	} ssaoData;

	struct VignetteData
	{
		float radius = 0.6f;
		float softness = 0.25f;
	} vignetteData;

private:
	PingPong m_pingPong;
	FrameBufferObject* m_gaussianBlurFbo;
	Texture* m_gaussianBlurInnerStepTexture;

	Program* m_gaussianBlurProgram;
	Program* m_colorCorrectionProgram;
	Program* m_fxaaProgram;
	Program* m_vignetteProgram;

	FrameBufferObject* m_ssaoFbo;
	Program* m_ssaoProgram;
	Program* m_ssaoBlurProgram;
	Texture* m_noiseTexture;
	Texture* m_ssaoTexture;
};

namespace_end

#endif