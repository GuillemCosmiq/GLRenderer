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

#include "render_panel.h"

#include "../../../engine.h"
#include "../../renderer/renderer.h"
#include "../../renderer/render_passes/lighting.h"
#include "../../renderer/render_passes/postprocessor.h"
#include "../../renderer/render_passes/screen_sampler.h"
#include "../../renderer/render_sources/screen_sampler_source.h"

namespace_begin

RenderPanel::RenderPanel()
	: BasePanel()
{
}

RenderPanel::RenderPanel(const std::string& name)
	: BasePanel(name)
{
}

void RenderPanel::Update()
{
	if (ImGui::Begin(m_name.c_str()))
	{
		if (ImGui::CollapsingHeader("PostProcessor", ImGuiTreeNodeFlags_DefaultOpen))
		{
			PostProcessor* postProcessor = Engine::Get()->renderer->postProcessor.get();
			if (ImGui::TreeNode("Bloom/Glow"))
			{
				ImGui::CheckboxFlags("Active ##BLOOM/GLOW", (unsigned int*)&postProcessor->filtersFlags, PostProcessor::FiltersFlags::Bloom);
				ImGui::SliderFloat("LOD 0", &postProcessor->bloomData.LODIntesities[0], 0.f, 5.f);
				ImGui::SliderFloat("LOD 1", &postProcessor->bloomData.LODIntesities[1], 0.f, 5.f);
				ImGui::SliderFloat("LOD 2", &postProcessor->bloomData.LODIntesities[2], 0.f, 5.f);
				ImGui::SliderFloat("LOD 3", &postProcessor->bloomData.LODIntesities[3], 0.f, 5.f);
				ImGui::SliderFloat("LOD 4", &postProcessor->bloomData.LODIntesities[4], 0.f, 5.f);
				ImGui::TreePop();

			}
			if (ImGui::TreeNode("Color correction"))
			{
				ImGui::CheckboxFlags("Active ##CC", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::ColorCorrection);
				if (ImGui::TreeNode("Gamma Correction"))
				{
					ImGui::CheckboxFlags("Active ##GC", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::GammaCorrection);
					ImGui::DragFloat("Gamma Value", &postProcessor->colorCorrectionData.gammaValue);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("HDR Tone mapping"))
				{
					ImGui::CheckboxFlags("Active ##TM", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::ToneMapping);
					ImGui::SliderFloat("Exposure", &postProcessor->colorCorrectionData.exposure, 0.f, 5.f);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSAO+"))
			{
				ImGui::CheckboxFlags("Active ##SSAO", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::SSAO);
				ImGui::SliderFloat("Power", &postProcessor->ssaoData.power, 0.f, 20.f);
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("FXAA"))
			{
				ImGui::CheckboxFlags("Active ##FXAA", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::FXAA);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Vignette"))
			{
				ImGui::CheckboxFlags("Active ##VIGNETTE", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::Vignette);
				ImGui::SliderFloat("Radius", &postProcessor->vignetteData.radius, 0.f, 1.f);
				ImGui::SliderFloat("Softness", &postProcessor->vignetteData.softness, 0.f, 1.f);
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("Renderer Output", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ScreenSampler* screenSampler = Engine::Get()->renderer->screenSampler.get();
			bool currentActive[] = { false , false, false };
			currentActive[screenSampler->outputSelection] = true;
			if (ImGui::RadioButton("Final", currentActive[(int)ScreenSamplerSource::OutputSample::scene]))
				screenSampler->outputSelection = (int)ScreenSamplerSource::OutputSample::scene;
			else if (ImGui::RadioButton("Depth", currentActive[(int)ScreenSamplerSource::OutputSample::depth]))
				screenSampler->outputSelection = (int)ScreenSamplerSource::OutputSample::depth;
			else if (ImGui::RadioButton("SSAO", currentActive[(int)ScreenSamplerSource::OutputSample::ssao]))
				screenSampler->outputSelection = (int)ScreenSamplerSource::OutputSample::ssao;

			ImGui::Text("Bloom/Glow LODS");
			ImGui::RadioButton("0", true);
			ImGui::SameLine();
			ImGui::RadioButton("1", true);
			ImGui::SameLine();
			ImGui::RadioButton("2", true);
			ImGui::SameLine();
			ImGui::RadioButton("3", true);
			ImGui::SameLine();	
			ImGui::RadioButton("4", true);
		}
		if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
		{

		}
	}
	ImGui::End();
}

namespace_end
