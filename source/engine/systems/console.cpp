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

#include "console.h"

namespace_begin

Console::Console()
{
}

Console::~Console()
{
	m_commands.clear();
}

void Console::Initialize(const Config& config)
{
}

void Console::Shutdown()
{
}

void Console::Print(const ImVec4& color, const std::string& text)
{
	m_outputs.emplace_back(std::make_pair(ImVec4(color), std::string(text)));
}

void Console::Print(const ImVec4&& color, const std::string&& text)
{
	m_outputs.emplace_back(std::make_pair(ImVec4(color), std::move(text)));
}

const std::vector<std::pair<ImVec4, std::string>> Console::GetOutputs()
{
	return m_outputs;
}

void Console::RemoveCommand(const std::string&& name)
{
	m_commands.erase(std::move(name));
}

bool Console::CallCommand(const std::string& name, int argument)
{
	bool ret = false;
	auto& it = m_commands.find(name);
	if (it != m_commands.end())
	{
		m_commandsHistory.emplace(it->first);
		Print(ImVec4(1.f, 0.f, 0.f, 1.f), it->second(argument));
		ret = true;
	}

	if (!ret)
	{
		Print(ImVec4(1.f, 0.f, 0.f, 1.f), "No matching commands were found");
	}
	return ret;
}

bool Console::CallCommand(const std::string&& name, int argument)
{
	bool ret = false;
	auto& it = m_commands.find(name);
	if (it != m_commands.end())
	{
		m_commandsHistory.emplace(it->first);
		Print(ImVec4(0.f, 1.f, 0.f, 1.f), it->second(argument));
		ret = true;
	}

	if (!ret)
	{
		Print(ImVec4(1.f, 0.f, 0.f, 1.f), "No matching commands were found");
	}
	return ret;
}

bool Console::ParseString(const std::string& string, std::string& functor, std::vector<std::string>& args)
{
	bool ret = false;
	char separator = ' ';
	std::string strToParse = string;
	std::string::size_type pos = strToParse.find_first_of(separator);
	std::string::size_type initialPos = 0;
	args.clear();

	Print(ImVec4(0.f, 1.f, 0.f, 1.f), string);
	if (pos != std::string::npos)
	{
		ret = true;
		functor = strToParse.substr(initialPos, pos - initialPos);
		initialPos = pos + 1;
		pos = strToParse.find_first_of(separator, initialPos);
		while (pos != std::string::npos)
		{
			args.push_back(strToParse.substr(initialPos, pos - initialPos));
			initialPos = pos + 1;
			pos = strToParse.find_first_of(separator, initialPos);
		}
		args.push_back(strToParse.substr(initialPos, std::min(pos, strToParse.size()) - initialPos + 1));
	}
	else
	{
		Print(ImVec4(1.f, 0.f, 0.f, 1.f), std::string("Could not parse \"" + std::string(string.c_str()) + "\". Try \"help\" for help"));
	}
	return ret;
}

namespace_end
