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

#include "program.h"

namespace_begin

Program::Program()
	: m_programID(0)
{
}

uint32 Program::GetID() const
{
	return m_programID;
}

void Program::AttachVertexObject(const char* vertexShader)
{
	auto verObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(verObject, 1, &vertexShader, NULL);
	m_objectIDs.emplace_back(std::move(verObject));
}

void Program::AttachGeometryObject(const char* geometryShader)
{
	auto geomObject = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geomObject, 1, &geometryShader, NULL);
	m_objectIDs.emplace_back(std::move(geomObject));
}

void Program::AttachFragmentObject(const char* fragmentShader)
{
	auto fragObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragObject, 1, &fragmentShader, NULL);
	m_objectIDs.emplace_back(std::move(fragObject));
}

bool Program::CompileProgram()
{
	GLint success = 0;
	m_programID = glCreateProgram();

	for (auto& object : m_objectIDs)
	{
		glCompileShader(object);
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(object, GL_INFO_LOG_LENGTH, &maxLength);
			char* errorLog = new char[maxLength];
			glGetShaderInfoLog(object, maxLength, &maxLength, &errorLog[0]);
			return false;
		}
		glAttachShader(m_programID, object);
	}

	glLinkProgram(m_programID);

	for (auto& object : m_objectIDs)
	{
		glDetachShader(m_programID, object);
	}

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &maxLength);
		char* errorLog = new char[maxLength];
		glGetProgramInfoLog(m_programID, maxLength, &maxLength, &errorLog[0]);
		return false;
	}

	return true;
}

void Program::Free()
{
	assert(m_objectIDs.size() > 0);
	assert(m_programID != 0 && glIsProgram(m_programID));

	for (auto& object : m_objectIDs)
	{
		assert(glIsShader(object));
		glDeleteShader(object);
	}

	glDeleteProgram(m_programID);
	m_programID = 0;
}

void Program::Bind() const
{
	glUseProgram(m_programID);
}

void Program::SetUniformTexture(const char* var, int v0) const
{
	glUniform1i(glGetUniformLocation(m_programID, var), v0);
}

void Program::SetUniformInt(const char* var, int v0) const
{
	glUniform1i(glGetUniformLocation(m_programID, var), v0);
}

void Program::SetUniformFloat(const char* var, float v0) const
{
	glUniform1f(glGetUniformLocation(m_programID, var), v0);
}

void Program::SetUniformFloatArray(const char* var, float* array, int size) const
{
	glUniform1fv(glGetUniformLocation(m_programID, var), size, array);
}

void Program::SetUniformVec3Array(const char* var, float* array, int size) const
{
	glUniform3fv(glGetUniformLocation(m_programID, var), size, array);
}

void Program::SetUniformVec2(const char* var, float v0, float v1) const
{
	glUniform2f(glGetUniformLocation(m_programID, var), v0, v1);
}

void Program::SetUniformVec3(const char* var, float v0, float v1, float v2) const
{
	glUniform3f(glGetUniformLocation(m_programID, var), v0, v1, v2);
}

void Program::SetUniformMat3(const char* var, bool transpose, const float* data) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_programID, var), 1, transpose, data);
}

void Program::SetUniformMat4(const char* var, bool transpose, const float* data) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_programID, var), 1, transpose, data);
}

namespace_end