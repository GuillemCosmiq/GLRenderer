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

#include "gpu_profiler.h"

namespace_begin

GPUProfiler::GPUProfiler()
{
	for (int i = 0; i < 10; ++i)
	{
		m_queries[i].hashedName = 0;
		m_queries[i].elapsedTimeMs = 0.f;
		m_queries[i].openGlid = 0;
		m_queries[i].sleepy = true;
	}
}

GPUProfiler::~GPUProfiler()
{
	for (int i = 0; i < 10; ++i)
	{
		glDeleteQueries(1, &m_queries[i].openGlid);
	}
}

void GPUProfiler::CreateQueries()
{
	for (int i = 0; i < 10; ++i)
	{
		assert(m_queries[i].openGlid == 0);
		glGenQueries(1, &m_queries[i].openGlid);
	}
}

void GPUProfiler::SleepQueries()
{
	for (int i = 0; i < 10; ++i)
	{
		m_queries[i].sleepy = true;
	}
}

int GPUProfiler::StartQuery(const char* name)
{
	for (int i = 0; i < 10; ++i)
	{
		if (m_queries[i].sleepy)
		{
			m_queries[i].sleepy = false;
			m_queries[i].name = name;
			std::hash<std::string> hasher;
			m_queries[i].hashedName = hasher(name);
			assert(m_queries[i].openGlid != 0);
			glBeginQuery(GL_TIME_ELAPSED, m_queries[i].openGlid);
			return i;
		}
	}
	assert(!"No queries available");
}

void GPUProfiler::EndQuery(int id)
{
	int params;
	GLuint64 elapsedTime;
	glEndQuery(GL_TIME_ELAPSED);
	glGetQueryObjectiv(m_queries[id].openGlid, GL_QUERY_RESULT_AVAILABLE, &params);
	glGetQueryObjectui64v(m_queries[id].openGlid, GL_QUERY_RESULT, &elapsedTime);
	m_queries[id].elapsedTimeMs = elapsedTime / 1000000.0;
}

float GPUProfiler::GetQueryResult(const char* name) const
{
	std::hash<std::string> hasher;
	size_t hash = hasher(name);
	for (int i = 0; i < 10; ++i)
	{
		if (m_queries[i].hashedName == hash)
		{
			return  m_queries[i].elapsedTimeMs;
		}
	}
	return 0.f;
	assert(!"Name not matching an existing query");
}

void GPUProfiler::GetActiveQueries(std::vector<std::pair<std::string, float>>& result) const
{
	result.reserve(10);
	for (int i = 0; i < 10; ++i)
	{
		if (!m_queries[i].sleepy)
		{
			result.emplace_back(std::make_pair(m_queries[i].name, m_queries[i].elapsedTimeMs));
		}
	}
}

namespace_end
