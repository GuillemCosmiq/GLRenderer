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

#ifndef __LIGHTING_H__
#define __LIGHTING_H__

#include "../ping_pong.h"

namespace_begin

class Renderer;
class ResourceSystem;
class LightingSource;
class Texture;
class Program;

class Lighting
{
public:
	Lighting() = delete;
	Lighting(ResourceSystem& resSystem, const Renderer& renderer);
	virtual ~Lighting();

	void Destroy();
	void Render(const Renderer& renderer, const LightingSource& lightingSource);

	inline FrameBufferObject* GetHDRFbo() const { return m_lightAccumulationPP.GetFBO(); }
	inline const Texture* GetSceneHDR() const { return m_lightAccumulationPP.GetBackBuffer(); }
	inline const Texture* GetBloomHDR() const { return m_bloomTextureHDR; }

public:
	bool DebugCSM; // Debug first CSM light

private:
	PingPong m_lightAccumulationPP;
	Texture* m_bloomTextureHDR;
	Program* m_dirLightProgram;
	Program* m_pointLightProgram;
	Program* m_finalShadingProgram;
};

namespace_end

#endif