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

#ifndef __PERFORMANCE_PANEL_H__
#define __PERFORMANCE_PANEL_H__

#include "base_panel.h"

namespace_begin

class PerformancePanel : public BasePanel
{
public:
	PerformancePanel();
	PerformancePanel(const std::string& name);
	virtual void Update() override;

private:
	float m_lastsGeometryMs[50];
	float m_lastsPBRMs[50];
	float m_lastsEnvironmentMs[50];
	float m_lastsPostProcessMs[50];
	float m_lastsSampleToScreenMs[50];
};

namespace_end

#endif