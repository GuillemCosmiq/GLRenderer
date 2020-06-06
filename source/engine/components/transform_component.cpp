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

#include "transform_component.h"

#include "../engine.h"
#include "../entity.h"

namespace_begin

TransformComponent::TransformComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_transform(1.f)
	, m_prevTransform(1.f)
	, m_position(0.f)
	, m_rotation(1.f, 0.f, 0.f, 0.f)
	, m_scale(1.f)
	, m_dirty(false)
{
}

const glm::vec3& TransformComponent::GetPosition()
{
	if (m_dirty)
	{
		DecomposeMatrix();
		m_dirty = false;
	}
	return m_position;
}

const glm::quat& TransformComponent::GetRotation()
{
	if (m_dirty)
	{
		DecomposeMatrix();
		m_dirty = false;
	}
	return m_rotation;
}

const glm::vec3& TransformComponent::GetScale()
{
	if (m_dirty)
	{
		DecomposeMatrix();
		m_dirty = false;
	}
	return m_scale;
}

void TransformComponent::SetMatrix(const glm::mat4x4& transformation)
{
	m_prevTransform = m_transform;
	m_transform = transformation;
	m_dirty = true;
}

void TransformComponent::DecomposeMatrix()
{
	glm::vec3 skew; // unused but needed for decompose
	glm::vec4 perspective; // unused but needed for decompose
	glm::decompose(m_transform, m_scale, m_rotation, m_position, skew, perspective);
	m_rotation = glm::conjugate(m_rotation);
}

namespace_end