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

#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "base_component.h"

namespace_begin

class TransformComponent : public BaseComponent
{
public:
	TransformComponent() = delete;
	TransformComponent(std::shared_ptr<Entity> owner);

	const glm::mat4x4& GetMatrix() const { return m_transform; };
	const glm::mat4x4& GetPrevFrameMatrix() const { return m_prevTransform; };
	const glm::vec3& GetPosition();
	const glm::quat& GetRotation();
	const glm::vec3& GetScale();

	void SetMatrix(const glm::mat4x4& newTransform);

private:
	void DecomposeMatrix();

private:
	glm::mat4x4 m_transform;
	glm::mat4x4 m_prevTransform;
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;
	bool m_dirty;
};

namespace_end

#endif