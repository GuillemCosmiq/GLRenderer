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

#include "ping_pong.h"

#include "../resource_system/resource_system.h"
#include "../resource_system/resources/framebuffer_object.h"

namespace_begin

PingPong::PingPong()
	: m_fbo(nullptr)
	, m_index(0)
{
	memset(m_buffers, 0, SizeofArray(m_buffers));
}

PingPong::~PingPong()
{
	assert(m_fbo == nullptr);
	assert(m_index == 0);
}

void PingPong::CreateBuffers(ResourceSystem& resSystem, const glm::vec2& size, uint32 internalFormat, uint32 format, uint32 dataType, const void* data)
{
	for (int i = 0; i < SizeofArray(m_buffers); ++i)
	{
		m_buffers[i] = resSystem.Create<Texture>();
		m_buffers[i]->Create();
		m_buffers[i]->Bind(0);
		m_buffers[i]->DefineBuffer(size, internalFormat, format, dataType, data);
	}
}

void PingPong::DefineBuffersParameters(uint32 parameter, uint32 value)
{
	for (int i = 0; i < SizeofArray(m_buffers); ++i)
	{
		m_buffers[i]->Bind(0);
		m_buffers[i]->DefineParameter(parameter, value);
	}
}

void PingPong::Create(ResourceSystem& resSystem)
{
	assert(m_fbo == nullptr);
	assert(m_index == 0);
	m_fbo = resSystem.Create<FrameBufferObject>();
	m_fbo->Init();
	m_fbo->Bind();
	for (int i = 0; i < SizeofArray(m_buffers); ++i)
		m_fbo->AttachTarget(m_buffers[i], GL_COLOR_ATTACHMENT0 + i);

	uint32 attachment = GL_COLOR_ATTACHMENT0;
	m_fbo->DefineDrawAttachments(&attachment, 1);
}

void PingPong::Free()
{
	for (auto& buffer : m_buffers)
	{
		buffer->Free();
		buffer = nullptr;
	}

	m_fbo->Free();
	m_fbo = nullptr;
	m_index = 0;
}

void PingPong::ResetState()
{
	m_index = 0;
	uint32 attachment = GL_COLOR_ATTACHMENT0;
	m_fbo->DefineDrawAttachments(&attachment, 1);
}

void PingPong::ClearAttachments(uint32 operation)
{
	for (int i = 0; i < SizeofArray(m_buffers); ++i)
		m_fbo->AttachTarget(m_buffers[i], GL_COLOR_ATTACHMENT0 + i);

	uint32 attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	m_fbo->DefineDrawAttachments(attachments, 2);
	glClear(operation);
}

void PingPong::BindFBO() const
{
	m_fbo->Bind();
}

void PingPong::AttachExtraBuffer(const Texture* texture, uint32 type)
{
	m_fbo->AttachTarget(texture, type);
}

void PingPong::AttachExtraBuffer(const RenderBuffer* renderBuffer, uint32 type)
{
	m_fbo->AttachTarget(renderBuffer, type);
}

void PingPong::AttachExtraBuffer(const Cubemap* cubemap, uint32 type)
{
	m_fbo->AttachTarget(cubemap, type);
}

void PingPong::SwapBuffers()
{
	m_index++;
	uint32 attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	m_fbo->DefineDrawAttachments(&attachments[m_index % 2], 1);
}

FrameBufferObject* PingPong::GetFBO() const
{
	return m_fbo;
}

const Texture* PingPong::GetBackBuffer() const
{
	return m_buffers[m_index % 2];
}

const Texture* PingPong::GetFrontBuffer() const
{
	return m_buffers[(m_index - 1) % 2];
}

uint32 PingPong::GetIndex() const
{
	return m_index;
}

namespace_end
