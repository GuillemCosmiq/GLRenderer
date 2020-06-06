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

#ifndef __MESH_H__
#define __MESH_H__

#include "resource.h"

namespace_begin

class Mesh : public Resource
{
	NON_COPYABLE_CLASS(Mesh);

public:
	Mesh();

public:
	enum class Layout
	{
		invalid = -1,
		vert_array,
		indices,
		tri_strip
	};

	struct Vertex
	{
		union
		{
			float position[3];
			struct { float m_px, m_py, m_pz; };
		};
		union
		{
			float texCoords[2];
			struct { float m_tcs, m_tct; };
		};
		union
		{
			float normal[3];
			struct { float m_nx, m_ny, m_nz; };
		};
		union
		{
			float tangent[3];
			struct { float m_tx, m_ty, m_tz; };
		};
		union
		{
			float bitangent[3];
			struct { float m_bx, m_by, m_bz; };
		};
	};

public:
	Layout GetLayout() const;

	uint32 GetVBOID() const;
	uint32 GetVAOID() const;
	uint32 GetIBOID() const;
	void BindAndDraw() const;

	const std::vector<Vertex>& GetVertices() const;
	const std::vector<unsigned int>& GetIndices() const;

	void Load();
	void Free();

	void SetLayout(Layout layout);

	void AttachVertices(const Vertex* vertices, unsigned int numVertices); // TODO: Implement move semantics functions
	void AttachVertices(const std::vector<Vertex>& vertices);

	void AttachIndices(const unsigned int* indices, unsigned int numIndices);
	void AttachIndices(const std::vector<unsigned int>& indices);

private:
	std::vector<Vertex>	m_vertices;
	std::vector<unsigned int> m_indices;
	uint32 m_vbo;
	uint32 m_vao;
	uint32 m_ibo;
	Layout m_layout;
};

namespace_end

#endif