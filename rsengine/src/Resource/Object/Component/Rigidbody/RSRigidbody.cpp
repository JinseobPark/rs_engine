#include "pch.h"
#include "RSRigidbody.h"
#include "../../RSObject.h"

namespace RS_Component
{
	RSRigidbody::RSRigidbody(const RSRigidbody& other) : RSComponent(RSComponentType::CT_RIGIDBODY)
	{
    m_velocity = other.m_velocity;
    m_acceleration = other.m_acceleration;
    m_gravity = other.m_gravity;
    m_is_kinematic = other.m_is_kinematic;
    m_is_gravity = other.m_is_gravity;
	}


	void RSRigidbody::Update(const float dt)
	{
		// If Kinematic, do not update
		if (m_is_kinematic) 
			return;
		if (m_is_gravity)
			m_acceleration.y = -m_gravity;

		m_velocity += m_acceleration * dt;
		GetOwner()->GetTransform()->AddPosition(m_velocity * dt);
		m_acceleration = glm::vec3(0.0f);

		// bounce from y == 0.
    if (GetOwner()->GetTransform()->GetPosition().y <= -5.0f)
    {
      GetOwner()->GetTransform()->SetPosition(glm::vec3(GetOwner()->GetTransform()->GetPosition().x, -5.0f, GetOwner()->GetTransform()->GetPosition().z));
      m_velocity.y = -m_velocity.y;
    }
	}


}