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
#include "../../renderer/render_passes/postprocessor.h"

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
	PostProcessor* postProcessor = Engine::Get()->renderer->postProcessor.get();

	if (ImGui::Begin(m_name.c_str()))
	{
		if (ImGui::CollapsingHeader("PostProcessor"))
		{
			if (ImGui::TreeNode("Bloom/Glow"))
			{
				ImGui::CheckboxFlags("Enable Bloom/Glow", (unsigned int*)&postProcessor->filtersFlags, PostProcessor::FiltersFlags::Bloom);
				ImGui::DragInt("Iterations", &postProcessor->bloomData.iterations);
				ImGui::TreePop();

			}
			if (ImGui::TreeNode("Color Correction"))
			{
				ImGui::CheckboxFlags("Enable CC", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::ColorCorrection);
				ImGui::Checkbox("Gamma Correction", &postProcessor->colorCorrectionData.gammaCorrection);
				ImGui::DragFloat("Gamma Value", &postProcessor->colorCorrectionData.gamma);
				ImGui::Checkbox("Tone mapping", &postProcessor->colorCorrectionData.toneMapping);
				ImGui::DragFloat("Exposure", &postProcessor->colorCorrectionData.exposure);
				ImGui::TreePop();
			}
			if(ImGui::TreeNode("FXAA"))
			{
				ImGui::CheckboxFlags("Enable FXAA", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::FXAA);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Vignette"))
			{
				ImGui::CheckboxFlags("Enable Vignette", (unsigned int*)& postProcessor->filtersFlags, PostProcessor::FiltersFlags::Vignette);
				ImGui::DragFloat("Radius", &postProcessor->vignetteData.radius);
				ImGui::DragFloat("Softness", &postProcessor->vignetteData.softness);
				ImGui::TreePop();
			}
		}

	}
	ImGui::End();
}

namespace_end
