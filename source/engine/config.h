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

#ifndef __CONFIG_H__
#define __CONFIG_H__

namespace_begin

class Config
{
public:
	explicit Config(const std::string& fileNamePath);
	~Config() = default;

	const std::string& GetWinName() { return m_winName; }
	const std::string& GetDefaultScene() { return m_defaultScene; }
	const std::string& GetLogFilePath() { return m_logFilePath; }
	const std::string& GetGrahicsConfigPath() { return m_graphics_config_path; }
	const std::string& GetDebugUiConfigPath() { return m_debug_ui_config_path; }

private:
	std::string m_winName;
	std::string m_defaultScene;
	std::string m_logFilePath;
	std::string m_graphics_config_path;
	std::string m_debug_ui_config_path;
};

namespace_end

#endif