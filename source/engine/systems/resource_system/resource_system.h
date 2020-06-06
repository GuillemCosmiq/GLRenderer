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

#ifndef __RESOURCE_SYSTEM_H__
#define __RESOURCE_SYSTEM_H__

namespace_begin

class Config;
class Resource;

class ResourceSystem
{
public:
	ResourceSystem() = default;

	void Initialize(Config& config);
	void Shutdown();

	template <typename T>
	T* Create()
	{
		static_assert(std::is_base_of<Resource, T>::value);
		std::unique_ptr<T> resource = std::make_unique<T>();
		m_resources.emplace_back(std::move(resource));
		return static_cast<T*>(m_resources.back().get());
	}

	template <typename T>
	void Destroy(T* const ptr)
	{
		static_assert(std::is_base_of<Resource, T>::value);
		auto it = std::find_if(m_resources.begin(), m_resources.end(),
			[ptr](auto& element)
			{
				return element.get() == ptr;
			});
		assert(it != m_resources.end());
		m_resources.erase(resource);
	}

private:
	std::vector<std::unique_ptr<Resource>> m_resources;
};

namespace_end

#endif