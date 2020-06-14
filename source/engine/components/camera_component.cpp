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

#include "camera_component.h"

#include "../engine.h"
#include "../entity.h"
#include "../systems/input.h"
#include "../systems/renderer/renderer.h"

namespace_begin

CameraComponent::CameraComponent(std::shared_ptr<Entity> owner)
	: BaseComponent(owner)
	, m_kbEmitterID(0)
	, m_mouseEmitterID(0)
{
	m_frustum.fieldOfView = 90.f;
	m_frustum.aspectRatio = 720.f / 480.f;
	m_frustum.nearPlane = 0.1f;
	m_frustum.farPlane = 100.f;
	m_frustum.dirty = true;
	m_frustum.perspective = glm::mat4(1.f);

	m_viewController.cameraSpeed = 0.01f;
	m_viewController.view = glm::mat4(1.f);
	m_viewController.pos = glm::vec3(0.f, 0.f, 15.f);
	m_viewController.front = glm::vec3(0.f, 0.f, -1.f);
	m_viewController.up = glm::vec3(0.f, 1.f, 0.f);
	m_viewController.yaw = -90.f;
	m_viewController.pitch = 0.f;
	m_viewController.dirty = true;
}

void CameraComponent::AddedToScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->renderer->SetCamera(std::static_pointer_cast<CameraComponent>(shared_from_this()));

	m_kbEmitterID = Engine::Get()->input->keyboardEmitter.AddReceptor([this](const SDL_Keycode key, const Input::KeyState state)
		{
			ProcessKeyboard(key, state);
		});

	m_mouseEmitterID = Engine::Get()->input->mouseEmitter.AddReceptor([this](const Input::MousePayload payload)
		{
			ProcessMouse(payload.deltaX, payload.deltaY);
		});
}

void CameraComponent::RemovedFromScene(std::shared_ptr<Scene> scene)
{
	Engine::Get()->input->keyboardEmitter.RemoveListener(m_kbEmitterID);
	Engine::Get()->input->keyboardEmitter.RemoveListener(m_mouseEmitterID);
}

void CameraComponent::SetFieldOfView(float fov)
{
	m_frustum.fieldOfView = fov;
	m_frustum.dirty = true;
}

void CameraComponent::SetAspectRatio(float aspectRatio)
{
	m_frustum.aspectRatio = aspectRatio;
	m_frustum.dirty = true;
}

void CameraComponent::SetNearPlane(float nearPlane)
{
	m_frustum.nearPlane = nearPlane;
	m_frustum.dirty = true;
}

void CameraComponent::SetFarPlane(float farPlane)
{
	m_frustum.farPlane = farPlane;
	m_frustum.dirty = true;
}

void CameraComponent::ProcessKeyboard(const SDL_Keycode key, const Input::KeyState state)
{
	if (key == SDL_SCANCODE_W && state == Input::KeyState::repeat)
		m_viewController.pos += m_viewController.cameraSpeed * m_viewController.front;
	if (key == SDL_SCANCODE_S && state == Input::KeyState::repeat)
		m_viewController.pos -= m_viewController.cameraSpeed * m_viewController.front;
	if (key == SDL_SCANCODE_A && state == Input::KeyState::repeat)
		m_viewController.pos -= glm::normalize(glm::cross(m_viewController.front, m_viewController.up)) * m_viewController.cameraSpeed;
	if (key == SDL_SCANCODE_D && state == Input::KeyState::repeat)
		m_viewController.pos += glm::normalize(glm::cross(m_viewController.front, m_viewController.up)) * m_viewController.cameraSpeed;
	
	if (key == SDL_SCANCODE_E && state == Input::KeyState::repeat)
		m_viewController.pos.y += m_viewController.cameraSpeed * 1.f;
	if (key == SDL_SCANCODE_Q && state == Input::KeyState::repeat)
		m_viewController.pos.y -= m_viewController.cameraSpeed * 1.f;
	
	if (key == SDL_SCANCODE_LSHIFT && state == Input::KeyState::down)
		m_viewController.cameraSpeed *= 2.f;
	if (key == SDL_SCANCODE_LSHIFT && state == Input::KeyState::up)
		m_viewController.cameraSpeed *= 0.5f;

	m_viewController.dirty = true;
}

void CameraComponent::ProcessMouse(double deltaX, double deltaY)
{
	float sensitivity = 0.5f;
	
	m_viewController.yaw += deltaX * sensitivity;
	m_viewController.pitch -= deltaY * sensitivity;
	
	if (m_viewController.pitch > 89.0f)
		m_viewController.pitch = 89.0f;
	if (m_viewController.pitch < -89.0f)
		m_viewController.pitch = -89.0f;
	
	glm::vec3 tmpFront;
	tmpFront.x = cos(glm::radians(m_viewController.yaw)) * cos(glm::radians(m_viewController.pitch));
	tmpFront.y = sin(glm::radians(m_viewController.pitch));
	tmpFront.z = sin(glm::radians(m_viewController.yaw)) * cos(glm::radians(m_viewController.pitch));
	m_viewController.front = glm::normalize(tmpFront);

	m_viewController.dirty = true;
}

const glm::mat4x4& CameraComponent::GetViewMatrix()
{
	if (m_viewController.dirty)
	{
		m_viewController.view = glm::lookAt(m_viewController.pos, m_viewController.pos + m_viewController.front, m_viewController.up);
		m_viewController.dirty = false;
	}
	return m_viewController.view;
}

const glm::mat4x4& CameraComponent::GetNormalsViewMatrix()
{
	m_viewController.normalsView = glm::lookAt(glm::vec3(0, 0, 0), m_viewController.front, m_viewController.up);
	return m_viewController.normalsView;
}

const glm::mat4x4& CameraComponent::GetProjection()
{
	if (m_frustum.dirty)
	{
		m_frustum.perspective = glm::perspective((float)glm::radians(m_frustum.fieldOfView), m_frustum.aspectRatio, m_frustum.nearPlane, m_frustum.farPlane);
		m_frustum.dirty = false;
	}
	return m_frustum.perspective;
}

void CameraComponent::GetWorldSpaceFrustumCorners(std::vector<glm::vec3>& corners) const
{
	corners.clear();
	corners.reserve(8);

	glm::vec4 homogeneousCorners[8];
	homogeneousCorners[0] = glm::vec4(1, 1, 1, 1);
	homogeneousCorners[1] = glm::vec4(-1, 1, 1, 1);
	homogeneousCorners[2] = glm::vec4(1, -1, 1, 1);
	homogeneousCorners[3] = glm::vec4(-1, -1, 1, 1);
	homogeneousCorners[4] = glm::vec4(1, 1, -1, 1);
	homogeneousCorners[5] = glm::vec4(-1, 1, -1, 1);
	homogeneousCorners[6] = glm::vec4(1, -1, -1, 1);
	homogeneousCorners[7] = glm::vec4(-1, -1, -1, 1);

	glm::mat4 inverseProj = glm::inverse(m_frustum.perspective * m_viewController.view);
	for (int i = 0; i < 8; ++i)
	{
		homogeneousCorners[i] = inverseProj * homogeneousCorners[i];
		homogeneousCorners[i] /= homogeneousCorners[i].w;
		corners.emplace_back(std::move(homogeneousCorners[i]));
	}
}

namespace_end
