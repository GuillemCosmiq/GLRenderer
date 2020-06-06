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

#include "mesh.h"

namespace_begin

Mesh::Mesh()
	: m_vbo(0)
	, m_vao(0)
	, m_ibo(0)
	, m_layout(Layout::invalid)
{
}

Mesh::Layout Mesh::GetLayout() const
{
	return m_layout;
}

uint32 Mesh::GetVBOID() const
{
	return m_vbo;
}

uint32 Mesh::GetVAOID() const
{
	return m_vao;
}

uint32 Mesh::GetIBOID() const
{
	return m_ibo;
}

void Mesh::BindAndDraw() const
{
	glBindVertexArray(m_vao);

	switch (m_layout)
	{
	case Mesh::Layout::indices:
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, NULL);
		break;
	case Mesh::Layout::tri_strip:
		glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertices.size());
		break;
	case Mesh::Layout::vert_array:
		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
		break;
	};
}

const std::vector<Mesh::Vertex>& Mesh::GetVertices() const
{
	return m_vertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const
{
	return m_indices;
}

void Mesh::Load()
{
	assert(m_vertices.size() > 0);
	assert(m_vbo == 0);
	assert(m_ibo == 0);
	assert(m_vao == 0);
	assert(m_layout != Layout::invalid);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

	if (m_layout == Layout::indices)
	{
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, bitangent)));
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::Free()
{
	assert(m_vertices.size() > 0);
	assert(m_vbo > 0);
	assert(m_vao > 0);
	assert(m_layout != Layout::invalid);

	if (m_layout == Layout::indices)
	{
		assert(m_ibo > 0);
		glDeleteBuffers(1, (GLuint*)&m_ibo);
		m_ibo = 0;
	}
	glDeleteBuffers(1, (GLuint*)&m_vbo);
	glDeleteVertexArrays(1, (GLuint*)&m_vao);

	m_vbo = 0;
	m_vao = 0;
}

void Mesh::SetLayout(Layout layout)
{
	m_layout = layout;
}

void Mesh::AttachVertices(const Vertex* vertices, unsigned int numVertices)
{
	m_vertices.resize(numVertices);
	std::memcpy(m_vertices.data(), vertices, sizeof(Vertex) * numVertices);
}

void Mesh::AttachVertices(const std::vector<Vertex>& vertices)
{
	m_vertices = vertices;
}

void Mesh::AttachIndices(const unsigned int* indices, unsigned int numIndices)
{
	m_indices.resize(numIndices);
	std::memcpy(m_indices.data(), indices, numIndices * sizeof(unsigned int));
}

void Mesh::AttachIndices(const std::vector<unsigned int>& indices)
{
	m_indices = indices;
}

namespace_end