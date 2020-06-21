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

#ifndef __CAMERA_COMPONENT_H__
#define __CAMERA_COMPONENT_H__

#include "base_component.h"

#include "../systems/input.h"

namespace_begin

class CameraComponent : public BaseComponent
{
public:
	CameraComponent() = delete;
	CameraComponent(std::shared_ptr<Entity> owner);

	void AddedToScene(std::shared_ptr<Scene> scene) override;
	void RemovedFromScene(std::shared_ptr<Scene> scene) override;

	void SetFieldOfView(float fov);
	void SetAspectRatio(float aspectRatio);
	void SetNearPlane(float nearPlane);
	void SetFarPlane(float farPlane);

	float GetFieldOfView() const { return m_frustum.fieldOfView; }
	float GetAspectRatio() const { return m_frustum.aspectRatio; }
	float GetNearPlane() const { return m_frustum.nearPlane; }
	float GetFarPlane() const { return m_frustum.farPlane; }

	const glm::mat4x4& GetViewMatrix();
	const glm::mat4x4& GetNormalsViewMatrix();
	const glm::mat4x4& GetProjection();
	const glm::vec3& GetPos() const { return m_viewController.pos; }
	void GetWorldSpaceFrustumCorners(std::vector<glm::vec3>& corners);

private:
	void ProcessKeyboard(const SDL_Keycode key, const Input::KeyState state);
	void ProcessMouse(double deltaX, double deltaY);

private:
	int m_kbEmitterID;
	int m_mouseEmitterID;

	struct
	{
		float fieldOfView;
		float aspectRatio;
		float nearPlane;
		float farPlane;
		glm::mat4 perspective;
		bool dirty;
	} m_frustum;

	struct
	{
		float cameraSpeed;
		glm::mat4x4 view;
		glm::mat4x4 normalsView;
		glm::vec3 pos;
		glm::vec3 front;
		glm::vec3 up;
		float yaw;
		float pitch;
		bool dirty;
	} m_viewController;
};

namespace_end

#endif