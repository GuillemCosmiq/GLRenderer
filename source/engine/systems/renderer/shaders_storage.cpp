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

#include "shaders_storage.h"

namespace_begin

void ShadersStorage::ParseAndStore(const char* relative_path)
{
	// TODO: use relative path :)
	std::string path(relative_path);
	m_utils = ReadFile((path + "utils.frag").c_str());
	m_PBRutils = ReadFile((path + "pbr_utils.frag").c_str());
	m_defaultVert = ReadFile((path + "default.vert").c_str());
	m_geometryVert = ReadFile((path + "geometry.vert").c_str());
	m_geometryFrag = ReadFile((path + "geometry.frag").c_str());
	m_dirShadowsVert = ReadFile((path + "shadow_pass.vert").c_str());
	m_dirShadowsFrag = ReadFile((path + "shadow_pass.frag").c_str());
	m_pointShadowsVert = ReadFile((path + "point_shadow_pass.vert").c_str());
	m_pointShadowsGeom = ReadFile((path + "point_shadow_pass.geom").c_str());
	m_pointShadowsFrag = ReadFile((path + "point_shadow_pass.frag").c_str());
	m_environmentVert = ReadFile((path + "environment.vert").c_str());
	m_environmentFrag = ReadFile((path + "environment.frag").c_str());
	m_equirectangularToCubemapVert = ReadFile((path + "equirectangularToCubemap.vert").c_str());
	m_equirectangularToCubemapFrag = ReadFile((path + "equirectangularToCubemap.frag").c_str());
	m_irradianceConvolutionFrag = ReadFile((path + "irradianceConvolution.frag").c_str());
	m_prefilterEnvFrag = ReadFile((path + "prefilterEnv.frag").c_str());
	m_brdfVert = ReadFile((path + "brdf.vert").c_str());
	m_brdfFrag = ReadFile((path + "brdf.frag").c_str());
	m_directionalShadingFrag = ReadFile((path + "directional_shading.frag").c_str());
	m_pointShadingFrag = ReadFile((path + "point_shading.frag").c_str());
	m_finalShadingFrag = ReadFile((path + "final_shading.frag").c_str());
	m_blur = ReadFile((path + "blur.frag").c_str());
	m_mergeBloom = ReadFile((path + "merge_bloom.frag").c_str());
	m_colorCorrection = ReadFile((path + "color_correction.frag").c_str());
	m_fxaa = ReadFile((path + "fxaa.frag").c_str());
	m_vignetteFrag = ReadFile((path + "vignette.frag").c_str());
	m_samplerToScreenFrag = ReadFile((path + "screen_sampler.frag").c_str());
}

std::string ShadersStorage::ReadFile(const char* file_name) const
{
	std::ifstream infile{ file_name };
	return std::string { std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() };
}

namespace_end