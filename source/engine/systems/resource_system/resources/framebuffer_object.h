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

#ifndef __FRAMEBUFFER_OBJECT_H__
#define __FRAMEBUFFER_OBJECT_H__

#include "resource.h"

namespace_begin

class RenderBuffer;
class Texture;
class Cubemap;

class FrameBufferObject : public Resource
{
	NON_COPYABLE_CLASS(FrameBufferObject);
public:
	FrameBufferObject();

	uint32 GetID() const;

	void Init();
	void Free();
	void Bind() const;

	void DefineDrawAttachments(const uint32* attachments, uint32 num) const;
	void DefineReadAttachment(uint32 att) const;

	void AttachTarget(const RenderBuffer* target, uint32 type) const;
	void AttachTarget(const Texture* target, uint32 type, uint32 level) const;
	void AttachTarget(const Cubemap* target, uint32 type, uint32 level) const;
	void AttachTarget(const Cubemap* target, uint32 type, uint32 face, uint32 level) const;

private:
	uint32 m_ID;
};

namespace_end

#endif