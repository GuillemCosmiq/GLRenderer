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

#include "autorotation_component.h"
#include "transform_component.h"

#include "../engine.h"
#include "../entity.h"

namespace_begin

AutorotationComponent::AutorotationComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_strengthRadians(DEFAULT_ROTATION_STRENGH)
{
	assert(owner->GetComponent<TransformComponent>() != nullptr); // no transform component attached.
	transformCompRef = owner->GetComponent<TransformComponent>().get();
}

void AutorotationComponent::Update()
{
	glm::mat4 entityTransform = transformCompRef->GetMatrix();
	entityTransform = glm::rotate(entityTransform, m_strengthRadians, glm::vec3(0.f, 1.f, 0.f));
	transformCompRef->SetMatrix(entityTransform);
}

void AutorotationComponent::SetRotation(float rotation)
{
	m_strengthRadians = rotation;
}

float AutorotationComponent::GetRotation() const
{
	return m_strengthRadians;
}

namespace_end