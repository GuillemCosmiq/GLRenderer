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

#include "entity.h"

#include "components/base_component.h"

namespace_begin

Entity::Entity()
	: m_name("")
{
}

Entity::Entity(std::string& name)
	: m_name(name)
{
}

Entity::~Entity()
{
	m_components.clear();
}

void Entity::Update()
{
	for (const auto& m_component : m_components)
		m_component.second->Update();
}

void Entity::PostUpdate()
{
	for (const auto& m_component : m_components)
		m_component.second->PostUpdate();
}

void Entity::AddedToScene(std::shared_ptr<Scene> scene)
{
	for (const auto& m_component : m_components)
	{
		m_component.second->AddedToScene(scene);
	}
}

void Entity::RemovedFromScene(std::shared_ptr<Scene> scene)
{
	for (const auto& m_component : m_components)
	{
		m_component.second->RemovedFromScene(scene);
	}
}

namespace_end
