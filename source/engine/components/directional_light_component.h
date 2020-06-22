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

#ifndef __DIRECTIONAL_LIGHT_COMPONENT_H__
#define __DIRECTIONAL_LIGHT_COMPONENT_H__

#include "base_component.h"

namespace_begin

class Texture;

class DirectionalLightComponent : public BaseComponent
{
public:
	DirectionalLightComponent() = delete;
	DirectionalLightComponent(std::shared_ptr<Entity> owner);

	void AddedToScene(std::shared_ptr<Scene> scene) override;
	void RemovedFromScene(std::shared_ptr<Scene> scene) override;

	void SetDirection(const glm::vec3& direction);
	void SetColor(const glm::vec3& color);
	void SetShadowCasting(bool enable);
	void SetShadowMap(Texture* shadowMap[3]);
	const glm::vec3& GetDirection() const;
	const glm::vec3& GetColor() const;
	bool IsCastingShadows() const;
	void GetShadowMaps(Texture* shadowMaps[3]) const;

	void ComputeOrtoProjViewContainingOBB(glm::mat4& outOrtoProj, glm::mat4& outView, const std::vector<glm::vec3>& corners, float nearclip, float nearClipOffset, float farclip);
	void GetLightSpaceCascadesProjViewMatrix(glm::mat4x4 matrices[3]) const;

private:
	glm::vec3 m_direction;
	glm::vec3 m_color;
	bool m_castShadows;
	Texture* m_shadowMaps[3];
	glm::mat4x4 m_lightSpaceCascadesMatrices[3];
};

namespace_end

#endif