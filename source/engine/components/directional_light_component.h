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
class TextureArray;

class DirectionalLightComponent : public BaseComponent
{
public:
	DirectionalLightComponent() = delete;
	DirectionalLightComponent(std::shared_ptr<Entity> owner);

	void AddedToScene(std::shared_ptr<Scene> scene) override;
	void RemovedFromScene(std::shared_ptr<Scene> scene) override;

	void Enable(bool value);
	void SetDirection(const glm::vec3& direction);
	void SetColor(const glm::vec3& color);
	void SetShadowCasting(bool enable);
	void SetShadowMap(Texture* shadowMap[3]);
	void SetFrustumSplits(float frustumSplits[3]);
	bool IsEnabled() const;
	const glm::vec3& GetDirection() const;
	const glm::vec3& GetColor() const;
	bool IsCastingShadows() const;
	const TextureArray* GetShadowMap() const;
	void GetFrustumSplits(float frustumSplits[3]) const;
	void GetShadowMapArray(Texture* shadowMaps[3]) const;

	void ComputeOrtoProjViewContainingOBB(const std::vector<glm::vec3>& corners, float nearclip, float nearClipOffset, float farclip);
	void GetLightSpaceCascadesProjViewMatrix(glm::mat4x4 matrices[3]) const;

private:
	int m_cameraEmitterID;
	bool m_enabled;
	glm::vec3 m_direction;
	glm::vec3 m_color;

	struct
	{
		bool CastShadows;
		Texture* ShadowMaps[3];
		glm::mat4x4 LightSpaceCascadesMatrices[3];
		float FrustumSplits[3];
	} m_shadowData;
};

namespace_end

#endif