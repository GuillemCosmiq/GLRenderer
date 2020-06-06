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

#include "performance_panel.h"

#include "../../../engine.h"
#include "../../renderer/renderer.h"
#include "../../renderer/gpu_profiler.h"

namespace_begin

PerformancePanel::PerformancePanel()
	: BasePanel()
{
	std::memset(m_lastsGeometryMs, 0, IM_ARRAYSIZE(m_lastsGeometryMs));
	std::memset(m_lastsPBRMs, 0, IM_ARRAYSIZE(m_lastsPBRMs));
	std::memset(m_lastsEnvironmentMs, 0, IM_ARRAYSIZE(m_lastsEnvironmentMs));
	std::memset(m_lastsPostProcessMs, 0, IM_ARRAYSIZE(m_lastsPostProcessMs));
	std::memset(m_lastsSampleToScreenMs, 0, IM_ARRAYSIZE(m_lastsSampleToScreenMs));
}

PerformancePanel::PerformancePanel(const std::string& name)
	: BasePanel(name)
{
	std::memset(m_lastsGeometryMs, 0, IM_ARRAYSIZE(m_lastsGeometryMs));
	std::memset(m_lastsPBRMs, 0, IM_ARRAYSIZE(m_lastsPBRMs));
	std::memset(m_lastsEnvironmentMs, 0, IM_ARRAYSIZE(m_lastsEnvironmentMs));
	std::memset(m_lastsPostProcessMs, 0, IM_ARRAYSIZE(m_lastsPostProcessMs));
	std::memset(m_lastsSampleToScreenMs, 0, IM_ARRAYSIZE(m_lastsSampleToScreenMs));
}

void PerformancePanel::Update()
{
	GPUProfiler* profiler = Engine::Get()->renderer->GetGPUProfilerPtr();
	if (ImGui::Begin(m_name.c_str()))
	{
		ImGui::Text("CPU");
		ImGui::Separator();

		// TODO: This can be heavily improved but it works for now...
		ImGui::Text("GPU");
		for (int i = 0; i < 49; ++i)
		{
			m_lastsGeometryMs[i] = m_lastsGeometryMs[i + 1];
			m_lastsPBRMs[i] = m_lastsPBRMs[i + 1];
			m_lastsEnvironmentMs[i] = m_lastsEnvironmentMs[i + 1];
			m_lastsPostProcessMs[i] = m_lastsPostProcessMs[i + 1];
			m_lastsSampleToScreenMs[i] = m_lastsSampleToScreenMs[i + 1];
		}

		m_lastsGeometryMs[IM_ARRAYSIZE(m_lastsGeometryMs) - 1] = profiler->GetQueryResult("Geometry");
		m_lastsPBRMs[IM_ARRAYSIZE(m_lastsPBRMs) - 1] = profiler->GetQueryResult("PBR");
		m_lastsEnvironmentMs[IM_ARRAYSIZE(m_lastsEnvironmentMs) - 1] = profiler->GetQueryResult("Environment");
		m_lastsPostProcessMs[IM_ARRAYSIZE(m_lastsPostProcessMs) - 1] = profiler->GetQueryResult("PostProcessor");
		m_lastsSampleToScreenMs[IM_ARRAYSIZE(m_lastsSampleToScreenMs) - 1] = profiler->GetQueryResult("Sample to screen");

		float GPmsAvg = 0.f;
		float PBRPmsAvg = 0.f;
		float EPmsAvg = 0.f;
		float PPPmsAvg = 0.f;
		float STSPmsAvg = 0.f;
		for (int i = 0; i < 50; ++i)
		{
			GPmsAvg += m_lastsGeometryMs[i];
			PBRPmsAvg += m_lastsPBRMs[i];
			EPmsAvg += m_lastsEnvironmentMs[i];
			PPPmsAvg += m_lastsPostProcessMs[i];
			STSPmsAvg += m_lastsSampleToScreenMs[i];
		}
		GPmsAvg /= 50.f;
		PBRPmsAvg /= 50.f;
		EPmsAvg /= 50.f;
		PPPmsAvg /= 50.f;
		STSPmsAvg /= 50.f;

		ImGui::Text("Lasts 50 frames avg:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "%f ms", (GPmsAvg + PBRPmsAvg + EPmsAvg + PPPmsAvg + STSPmsAvg));

		ImGui::Text("Geometry pass:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "%f ms", m_lastsGeometryMs[IM_ARRAYSIZE(m_lastsGeometryMs) - 1]);
		ImGui::PlotHistogram("##gpp", m_lastsGeometryMs, IM_ARRAYSIZE(m_lastsGeometryMs), 0.f, "Ms", 0.f, GPmsAvg * 2.f, ImVec2(0, 50));

		ImGui::Text("PBR pass:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "%f ms", m_lastsPBRMs[IM_ARRAYSIZE(m_lastsPBRMs) - 1]);
		ImGui::PlotHistogram("##pbrpp", m_lastsPBRMs, IM_ARRAYSIZE(m_lastsPBRMs), 0, "Ms", 0.f, PBRPmsAvg * 2.f, ImVec2(0, 50));

		ImGui::Text("Environment pass:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "%f ms", m_lastsEnvironmentMs[IM_ARRAYSIZE(m_lastsEnvironmentMs) - 1]);
		ImGui::PlotHistogram("##epp", m_lastsEnvironmentMs, IM_ARRAYSIZE(m_lastsEnvironmentMs), 0.f, "Ms", 0.f, EPmsAvg * 2.f, ImVec2(0, 50));

		ImGui::Text("Postprocess pass:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "%f ms", m_lastsPostProcessMs[IM_ARRAYSIZE(m_lastsPostProcessMs) - 1]);
		ImGui::PlotHistogram("##pppp", m_lastsPostProcessMs, IM_ARRAYSIZE(m_lastsPostProcessMs), 0.f, "Ms", 0.f, PPPmsAvg * 2.f, ImVec2(0, 50));

		ImGui::Text("Sample to screen pass:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "%f ms", m_lastsSampleToScreenMs[IM_ARRAYSIZE(m_lastsSampleToScreenMs) - 1]);
		ImGui::PlotHistogram("##sspp", m_lastsSampleToScreenMs, IM_ARRAYSIZE(m_lastsSampleToScreenMs), 0.f, "Ms", 0.f, STSPmsAvg * 2.f, ImVec2(0, 50));
	}
	ImGui::End();
}

namespace_end
