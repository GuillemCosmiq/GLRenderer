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

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

namespace_begin

class Config;

class Console
{
public:
	using Command = std::function<std::string(int)>;
public:
	Console();
	~Console();
	void Initialize(const Config& config);
	void Shutdown();

	void Print(const ImVec4& color, const std::string& text);
	void Print(const ImVec4&& color, const std::string&& text);
	const std::vector<std::pair<ImVec4, std::string>> GetOutputs();

	template<typename A>
	void AddCommand(std::string&& name, A&& method)
	{
		m_commands.insert(std::make_pair(std::move(name), std::forward<A>(method)));
	}

	void RemoveCommand(const std::string&& name);
	bool CallCommand(const std::string& name, int argument);
	bool CallCommand(const std::string&& name, int argument);
	bool ParseString(const std::string& string, std::string& functor, std::vector<std::string>& args);

private:
	std::vector<std::pair<ImVec4, std::string>> m_outputs;
	std::map<std::string, Command> m_commands;
	std::queue<std::string> m_commandsHistory;
};

namespace_end

#endif