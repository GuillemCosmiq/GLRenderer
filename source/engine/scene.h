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

#ifndef __SCENE_H__
#define __SCENE_H__

namespace_begin

class Config;
class ResourceSystem;
class Entity;
class Texture;

class Scene : public std::enable_shared_from_this<Scene>
{
public:
	Scene();
	virtual ~Scene();

	void Initialize(Config& config, ResourceSystem& resSystem);
	void Update();

	void AddEntity(std::shared_ptr<Entity> entity);
	void RemoveEntity(std::shared_ptr<Entity> entity);

	void SetCurrentSwitchableObjScene(const std::string&);
	const std::map<const std::string, std::shared_ptr<Entity>>& GetLoadedSwitchableObjScenes() const;
	void LoadAndStoreSwitchableObjScenes(ResourceSystem& resSystem, const std::string& name, const std::string& path, const glm::vec3& scale = glm::vec3(1.f, 1.f, 1.f));
	void SetRotationStrenghOfObjScenes(float value);

	void SetCurrentEnvironmentFromLoadedMap(const std::string& name);
	const std::map<const std::string, Texture*>& GetLoadedEnvironments() const;
	void LoadAndStoreEnvironment(ResourceSystem& resSystem, const std::string& name, const std::string& path);

private:
	std::string m_name;
	std::vector<std::shared_ptr<Entity>> m_entities;

	std::string m_currentSwitchableObjScene;
	std::map<const std::string, std::shared_ptr<Entity>> m_loadedSwitchableObjScenes;
	std::map<const std::string, Texture*> m_loadedEnvironmentMaps;
};

namespace_end

#endif