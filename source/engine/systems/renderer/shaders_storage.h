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

#ifndef _SHADERS_STORAGE_H__
#define _SHADERS_STORAGE_H__

namespace_begin

class ShadersStorage
{
public:
	ShadersStorage() = default;
	void ParseAndStore(const char*);
	std::string ReadFile(const char*) const;

	inline const std::string& GetUtils() const { return m_utils; }
	inline const std::string& GetPBRutils() const { return m_PBRutils; }
	inline const std::string& GetDefaultVert() const { return m_defaultVert; }
	inline const std::string& GetGeometryVert() const { return m_geometryVert; }
	inline const std::string& GetGeometryFrag() const { return m_geometryFrag; }
	inline const std::string& GetDirShadowsVert() const { return m_dirShadowsVert; }
	inline const std::string& GetDirShadowsFrag() const { return m_dirShadowsFrag; }
	inline const std::string& GetPointShadowsVert() const { return m_pointShadowsVert; }
	inline const std::string& GetPointShadowsGeom() const { return m_pointShadowsGeom; }
	inline const std::string& GetPointShadowsFrag() const { return m_pointShadowsFrag; }
	inline const std::string& GetEnvironmentVert() const { return m_environmentVert; }
	inline const std::string& GetEnvironmentFrag() const { return m_environmentFrag; }
	inline const std::string& GetEquirectangularToCubemapVert() const { return m_equirectangularToCubemapVert; }
	inline const std::string& GetEquirectangularToCubemapFrag() const { return m_equirectangularToCubemapFrag; }
	inline const std::string& GetIrradianceConvolutionFrag() const { return m_irradianceConvolutionFrag; }
	inline const std::string& GetPrefilterEnvironmentFrag() const { return m_prefilterEnvFrag; }
	inline const std::string& GetBrdfVert() const { return m_brdfVert; }
	inline const std::string& GetBrdfFrag() const { return m_brdfFrag; }
	inline const std::string& GetSSAOFrag() const { return m_ssaoFrag; }
	inline const std::string& GetSSAOBlurFrag() const { return m_ssaoBlurFrag; }
	inline const std::string& GetDirectionalShadingFrag() const { return m_directionalShadingFrag; }
	inline const std::string& GetPointShadingFrag() const { return m_pointShadingFrag; }
	inline const std::string& GetFinalShadingFrag() const { return m_finalShadingFrag; }
	inline const std::string& GetBlurFrag() const { return m_blur; }
	inline const std::string& GetMergeBloomFrag() const { return m_mergeBloom; }
	inline const std::string& GetColorCorrectionFrag() const { return m_colorCorrection; }
	inline const std::string& GetFXAAfrag() const { return m_fxaa; }
	inline const std::string& GetVignetteFrag() const { return m_vignetteFrag; }
	inline const std::string& GetSamplerToScreenFrag() const { return m_samplerToScreenFrag; }

private:
	std::string m_utils;
	std::string m_PBRutils;
	std::string m_defaultVert;
	std::string m_geometryVert;
	std::string m_geometryFrag;
	std::string m_dirShadowsVert;
	std::string m_dirShadowsFrag;
	std::string m_pointShadowsVert;
	std::string m_pointShadowsGeom;
	std::string m_pointShadowsFrag;
	std::string m_environmentVert;
	std::string m_environmentFrag;
	std::string m_equirectangularToCubemapVert;
	std::string m_equirectangularToCubemapFrag;
	std::string m_irradianceConvolutionFrag;
	std::string m_prefilterEnvFrag;
	std::string m_brdfVert;
	std::string m_brdfFrag;
	std::string m_ssaoFrag;
	std::string m_ssaoBlurFrag;
	std::string m_directionalShadingFrag;
	std::string m_pointShadingFrag;
	std::string m_finalShadingFrag;
	std::string m_blur;
	std::string m_mergeBloom;
	std::string m_colorCorrection;
	std::string m_fxaa;
	std::string m_vignetteFrag;
	std::string m_samplerToScreenFrag;
};

namespace_end

#endif