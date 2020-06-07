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

#ifndef __PING_PONG_H__
#define __PING_PONG_H__

#include "../resource_system/resources/texture.h"

namespace_begin

class FrameBufferObject;
class ResourceSystem;

class RenderBuffer;
class Cubemap;

class PingPong
{
public:
	PingPong();
	~PingPong();

	void CreateBuffers(ResourceSystem& resSystem, const glm::vec2& size, uint32 internalFormat, uint32 format, uint32 dataType,
		const void* data = NULL);
	void DefineBuffersParameters(uint32 parameter, uint32 value);
	void Create(ResourceSystem& resSystem);
	void Free();
	void ResetState();
	void ClearAttachments(uint32 operation);
	void BindFBO() const;
	void AttachExtraBuffer(const Texture* texture, uint32 type);
	void AttachExtraBuffer(const RenderBuffer* texture, uint32 type);
	void AttachExtraBuffer(const Cubemap* texture, uint32 type);
	void SwapBuffers();

	FrameBufferObject* GetFBO() const;
	const Texture* GetBackBuffer() const;
	const Texture* GetFrontBuffer() const;
	uint32 GetIndex() const;

private:
	FrameBufferObject* m_fbo;
	Texture* m_buffers[2];
	uint32 m_index;
};

namespace_end

#endif