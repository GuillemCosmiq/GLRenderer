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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "resource.h"

namespace_begin

class Texture : public Resource
{
	NON_COPYABLE_CLASS(Texture);

public:
	Texture();
	~Texture();

	void Create();
	void Free();
	void Bind(int textureIndex) const;
	void DefineParameter(uint32 parameter, uint32 value) const;
	void DefineBuffer(const glm::vec2& size, uint32 level, uint32 internalFormat, uint32 format, uint32 dataType, const void* data);
	void GenerateMipMaps() const;

	uint32 GetID() const;
	const glm::vec2& GetCurrentBufferSize();

private:
	uint32 m_ID;
	glm::vec2 m_currentBufferSize;
};

namespace_end

#endif