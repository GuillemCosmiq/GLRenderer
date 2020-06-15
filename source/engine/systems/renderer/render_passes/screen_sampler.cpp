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

#include "screen_sampler.h"
#include "../render_sources/screen_sampler_source.h"

#include "../renderer.h"
#include "..//shaders_storage.h"
#include "../../resource_system/resource_system.h"
#include "../../resource_system/resources/framebuffer_object.h"
#include "../../resource_system/resources/program.h"
#include "../../resource_system/resources/mesh.h"
#include "../../resource_system/resources/texture.h"

namespace_begin

ScreenSampler::ScreenSampler(ResourceSystem& resSystem, const Renderer& renderer)
{
	outputSelection = 0;

	m_screenSampler = resSystem.Create<Program>();
	m_screenSampler->AttachVertexObject(renderer.shaderStorage->GetDefaultVert().c_str());
	m_screenSampler->AttachFragmentObject(renderer.shaderStorage->GetSamplerToScreenFrag().c_str());
	m_screenSampler->CompileProgram();
	m_screenSampler->Bind();
	m_screenSampler->SetUniformTexture("sceneSample", 0);
}

ScreenSampler::~ScreenSampler() {}

void ScreenSampler::Destroy()
{
	m_screenSampler->Free();
}

void ScreenSampler::Render(const Renderer& renderer, const ScreenSamplerSource& source)
{
	glm::vec2 viewport = renderer.GetViewport();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_screenSampler->Bind();
	source.SceneSample[outputSelection]->Bind(0);

	m_screenSampler->SetUniformVec2("viewport", viewport.x, viewport.y);
	m_screenSampler->SetUniformInt("outputSelection", outputSelection);
	renderer.quad->BindAndDraw();
}

namespace_end