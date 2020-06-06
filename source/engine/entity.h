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

#ifndef __ENTITY_H__
#define __ENTITY_H__

namespace_begin

class Scene;
class BaseComponent;

class Entity final : public std::enable_shared_from_this<Entity>
{
public:
	Entity();
	Entity(std::string& name);
	~Entity();

	void Update();

	void SetName(std::string& name) { m_name = name; }
	std::string GetName() { return m_name; }

	void AddedToScene(std::shared_ptr<Scene> scene);
	void RemovedFromScene(std::shared_ptr<Scene> scene);

	template <class T>
	std::shared_ptr<T> AddComponent()
	{
		std::string type = typeid(T).name();
		if (m_components.find(type) == m_components.end())
		{
			std::shared_ptr<T> Tcomponent = std::make_shared<T>(shared_from_this());
			std::shared_ptr<BaseComponent> component = std::static_pointer_cast<BaseComponent>(Tcomponent);
			m_components.insert(std::pair<std::string, std::shared_ptr<BaseComponent>>(std::move(type), component));
			return Tcomponent;
		}
		assert(!"Component already created on this entity");
		return std::shared_ptr<T>();
	}

	template <class T>
	bool AddComponent(std::shared_ptr<T> componentToAdd)
	{
		std::string type = typeid(T).name();
		if (m_components.find(type) == m_components.end())
		{
			std::shared_ptr<BaseComponent> component = std::static_pointer_cast<BaseComponent>(componentToAdd);
			m_components.insert(std::pair<std::string, std::shared_ptr<BaseComponent>>(std::move(type), component));
			return true;
		}
		return false;
	}

	template <class T>
	std::shared_ptr<T> GetComponent()
	{
		auto& pair = m_components.find(typeid(T).name());
		if (pair != m_components.end())
		{
			return std::static_pointer_cast<T>(pair->second);
		}
		return std::shared_ptr<T>();
	}

private:
	std::string m_name;
	std::map<std::string, std::shared_ptr<BaseComponent>> m_components;
};

namespace_end

#endif