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

#include "automovement_component.h"
#include "transform_component.h"

#include "../engine.h"
#include "../entity.h"

namespace_begin

AutomovementComponent::AutomovementComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_strength(DEFAULT_MOVEMENT_STRENGH)
	, m_amplitude(DEFAULT_MOVEMENT_AMPLITUDE)
	, m_time(0.f)
{
	assert(owner->GetComponent<TransformComponent>() != nullptr); // no transform component attached.
	transformCompRef = owner->GetComponent<TransformComponent>().get();
}

void AutomovementComponent::Update()
{
	glm::vec3 position = transformCompRef->GetPosition();
	position.x = glm::sin(m_time * m_strength) * m_amplitude;
	transformCompRef->SetPosition(position);
	m_time += FRAME_MS;
}

void AutomovementComponent::SetAmplitude(float amplitude)
{
	m_amplitude = amplitude;
}

void AutomovementComponent::SetStrengh(float strengh)
{
	m_strength = strengh;
}

float AutomovementComponent::GetAmplitude() const
{
	return m_amplitude;
}

float AutomovementComponent::GetStrengh() const
{
	return m_strength;
}

namespace_end