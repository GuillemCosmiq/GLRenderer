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

#ifndef __DRAWABLE_COMPONENT_H__
#define __DRAWABLE_COMPONENT_H__

#include "base_component.h"

namespace_begin

class Mesh;
class Texture;

class DrawableComponent : public BaseComponent
{
public:
	DrawableComponent() = delete;
	DrawableComponent(std::shared_ptr<Entity> owner);

	void AddedToScene(std::shared_ptr<Scene> scene) override;
	void RemovedFromScene(std::shared_ptr<Scene> scene) override;

	void SetMesh(Mesh* mesh) { m_mesh = mesh; };
	void SetAlbedoMap(Texture* albedoMap) { m_albedoMap = albedoMap; };
	void SetNormalMap(Texture* normalMap) { m_normalMap = normalMap; };
	void SetMetallicMap(Texture* metallicMap) { m_metallicMap = metallicMap; };
	void SetRoughnessMap(Texture* roughnessMap) { m_roughnessMap = roughnessMap; };
	void SetAOMap(Texture* AOMap) { m_AOMap = AOMap; };
	void SetNumTiles(int value) { m_numTiles = value; };
	void SetIfMaskedForMotionBlur(bool enable) { m_maskedForMotionBlur = enable; };

	Mesh* GetMesh() const { return m_mesh; };
	Texture* GetAlbedoMap() const { return m_albedoMap; };
	Texture* GetNormalMap() const { return m_normalMap; };
	Texture* GetMetallicMap() const { return m_metallicMap; };
	Texture* GetRoughnessMap() const { return m_roughnessMap; };
	Texture* GetAOMap() const { return m_AOMap; };
	
	int GetNumTiles() const { return m_numTiles; }
	bool IsMaskedForMotionBlur() const { return m_maskedForMotionBlur; };

private:
	Mesh* m_mesh;
	Texture* m_albedoMap;
	Texture* m_normalMap;
	Texture* m_metallicMap;
	Texture* m_roughnessMap;
	Texture* m_AOMap;

	int m_numTiles;
	bool m_maskedForMotionBlur;
};

namespace_end

#endif