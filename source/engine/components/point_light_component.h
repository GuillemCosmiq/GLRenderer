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

#ifndef __POINT_LIGHT_COMPONENT_H__
#define __POINT_LIGHT_COMPONENT_H__

#include "base_component.h"

namespace_begin

class Cubemap;

class PointLightComponent : public BaseComponent
{
public:
	PointLightComponent() = delete;
	PointLightComponent(std::shared_ptr<Entity> owner);

	void AddedToScene(std::shared_ptr<Scene> scene) override;
	void RemovedFromScene(std::shared_ptr<Scene> scene) override;

	void SetRadius(const float radius);
	void SetColor(const glm::vec3& color);
	void SetShadowMap(Cubemap* shadowMap);
	void SetShadowCasting(bool enable);
	float GetRadius() const { return m_radius; }
	glm::vec3 GetColor() const { return m_color; }
	Cubemap* GetShadowMap() const { return m_shadowMap; }
	bool IsCastingShadows() const { return m_castShadows; }

	void GetLightSpaceTransformationMatrices(std::vector<glm::mat4>& matrices, const glm::vec3& position);

private:
	float m_radius;
	glm::vec3 m_color;
	Cubemap* m_shadowMap;
	bool m_castShadows;
};

namespace_end

#endif