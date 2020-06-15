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

#ifndef __ENVIRONMENT_PASS_H__
#define __ENVIRONMENT_PASS_H__

namespace_begin

class Renderer;
class ResourceSystem;
class FrameBufferObject;
class RenderBuffer;
class Texture;
class Cubemap;
class Program;

class EnvironmentPass
{
public:
	EnvironmentPass() = delete;
	EnvironmentPass(ResourceSystem& resSystem, const Renderer& renderer);
	virtual ~EnvironmentPass();

	void Destroy();
	void Render(const Renderer& renderer);

	void ComputeCubemapFromEquirectangularMap(const Renderer& renderer);
	void ComputeIrradianceMap(const Renderer& renderer);
	void ComputePrefilterMap(const Renderer& renderer);
	void ComputeBRDFMap(const Renderer& renderer);

	void SetCaptureMatrices(glm::mat4& projection, glm::mat4(&captureViews)[6]);
	void SetEquirectangularEnv(Texture* equirectangularMap);

	void ComputeSSAO(const Renderer& renderer);

public:
	FrameBufferObject* m_fbo;
	Program* m_skyboxProgram;
	Program* m_equirectangularToCubemapProgram;
	Program* m_irradianceConvolutionProgram;
	Program* m_prefilterEnvProgram;
	Program* m_brdfProgram;
	Texture* m_equirectangularEnv;
	Cubemap* m_environmentCubemap;
	Cubemap* m_irradianceCubemap;
	Cubemap* m_prefilterCubemap;
	Texture* m_brdfLUTTexture;
	bool needComputeEnvironmentMaps;

	glm::mat4 m_captureProjection;
	glm::mat4 m_captureViews[6];
};

namespace_end

#endif