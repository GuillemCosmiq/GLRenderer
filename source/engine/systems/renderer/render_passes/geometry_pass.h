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

#ifndef __GEOMETRY_PASS_H__
#define __GEOMETRY_PASS_H__

namespace_begin

class Renderer;
class ResourceSystem;
class GeometrySource;
class FrameBufferObject;
class Texture;
class Program;

class GeometryPass
{
public:
	GeometryPass() = delete;
	GeometryPass(ResourceSystem& resSystem, const Renderer& renderer);
	virtual ~GeometryPass();

	void Destroy();
	void Render(const Renderer& renderer, const GeometrySource& geometrySource);

	inline FrameBufferObject* GetTarget() const { return m_fbo; };
	inline const Texture* GetAlbedo() const { return m_albedoTexture; };
	inline const Texture* GetNormals() const { return m_normalsTexture; };
	inline const Texture* GetMaterial() const { return m_materialTexture; };
	inline const Texture* GetDepth() const { return m_depthTexture; };

private:
	FrameBufferObject* m_fbo;
	Program* m_geometryProgram;
	Texture* m_albedoTexture;
	Texture* m_normalsTexture;
	Texture* m_materialTexture;
	Texture* m_depthTexture;
};

namespace_end

#endif