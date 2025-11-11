/******************************************************************************/
/*!
\file   RSRigidbody.h
\author Jinseob Park
\date   2024/08/01

This file contains rigidbody component for object,
which includes velocity, acceleration, drag, gravity, and kinematic.

*/
/******************************************************************************/
#ifndef RS_RIGIDBODY_H_
#define RS_RIGIDBODY_H_

#include "../RSComponent.h"
#include "glm/glm.hpp"

/**
 * @brief Object Component namespace
 */
namespace RS_Component
{
	/**
	 * @brief Rigidbody class for object component.
	 * 	 * It derives from RSComponent.
	 */
	class RSRigidbody : public RSComponent
	{
	public:
		RSRigidbody() : RSComponent(RSComponentType::CT_RIGIDBODY) {}
		~RSRigidbody() override = default;

    // Deep copy constructor
    RSRigidbody(const RSRigidbody& other);

    RSComponent* Clone() const override { return new RSRigidbody(*this); }

		/**
		 * @brief Get name of component
		 * @return name of component. (RSRigidbody)
		 */
		std::string GetName() override { return "RSRigidbody"; }

		void Initialize() override {}

		/**
		 * @brief Update Rigidbody. Currently, it only updates position, velocity, and acceleration by gravity.
		 * @param dt delta time
		 */
		void Update(float dt);

		/**
		 * @brief Set velocity of Rigidbody
		 * @param velocity_ velocity(vec3)
		 */
		void SetVelocity(const glm::vec3 velocity_) { m_velocity = velocity_; }

		/**
		 * @brief Set gravity acceleration of Rigidbody
		 * @param gravity_ gravity acceleration(float)
		 */
		void SetGravity(const float gravity_) { m_gravity = gravity_; }

	private:
    glm::vec3 m_velocity = glm::vec3(0.0f); ///< Velocity of Rigidbody
    glm::vec3 m_acceleration = glm::vec3(0.0f); ///< Acceleration of Rigidbody
		
		//float m_drag = 0.0f; // Not implemented yet
    float m_gravity = 9.8f; ///< Gravity acceleration of Rigidbody
    bool m_is_gravity = true; ///< Is gravity applied to Rigidbody

    bool m_is_kinematic = false; ///< Is kinematic Rigidbody
	};
}

#endif // !RS_RIGIDBODY_H_