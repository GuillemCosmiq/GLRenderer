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

#include "framebuffer_object.h"

#include "render_buffer.h"
#include "texture.h"
#include "cubemap.h"

namespace_begin

FrameBufferObject::FrameBufferObject()
	: m_ID(0)
{
}

uint32 FrameBufferObject::GetID() const
{
	return m_ID;
}

void FrameBufferObject::Init()
{
	glGenFramebuffers(1, &m_ID);
}

void FrameBufferObject::Free()
{
	glDeleteFramebuffers(1, &m_ID);
}

void FrameBufferObject::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FrameBufferObject::DefineDrawAttachments(uint32* attachments, uint32 num)
{
	if (num > 1)
		glDrawBuffers(num, attachments);
	else
		glDrawBuffer(*attachments);
}

void FrameBufferObject::DefineReadAttachment(uint32 attachment)
{
	glReadBuffer(attachment);
}

void FrameBufferObject::AttachTarget(const RenderBuffer* target, uint32 type)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, target->GetID());
	GLenum result = 0;
	result = glGetError();
}

void FrameBufferObject::AttachTarget(const Texture* target, uint32 type)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, target->GetID(), 0);
	GLenum result = 0;
	result = glGetError();
}

void FrameBufferObject::AttachTarget(const Cubemap* target, uint32 type)
{
	glFramebufferTexture(GL_FRAMEBUFFER, type, target->GetID(), 0);
	GLenum result = 0;
	result = glGetError();
}

void FrameBufferObject::AttachTarget(const Cubemap* target, uint32 type, uint32 face, uint32 mipMapLevel)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, target->GetID(), mipMapLevel);
	GLenum result = 0;
	result = glGetError();
}

namespace_end