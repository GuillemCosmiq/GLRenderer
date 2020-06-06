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

#ifndef __DEBUG_UI_SYSTEM_H__
#define __DEBUG_UI_SYSTEM_H__

namespace_begin

class BasePanel;
class Config;

class DebugUISystem
{
public:
	DebugUISystem();
	void Initialize(Config& config);
	void Update();
	void Shutdown();

	void UpdateMainMenu();
	void UpdatePanels();

	template <class T>
	std::weak_ptr<T> AddPanel()
	{
		std::string type = typeid(T).name();
		if (m_panels.find(type) == m_panels.end())
		{
			std::shared_ptr<T> Tpanel = std::make_shared<T>(type);
			std::shared_ptr<BasePanel> panel = std::static_pointer_cast<BasePanel>(Tpanel);
			m_panels.insert(std::pair<std::string, std::shared_ptr<BasePanel>>(std::move(type), panel));
			return Tpanel;
		}
		return std::shared_ptr<T>();
	}

private:
	 std::map<std::string, std::shared_ptr<BasePanel>> m_panels;
	 bool m_enabled;
	 int m_enabledInputHandler;
};

namespace_end

#endif