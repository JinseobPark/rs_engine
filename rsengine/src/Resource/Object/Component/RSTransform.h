/******************************************************************************/
/*!
\file   RSTransform.h
\author Jinseob Park
\date   2024/08/01

This file contains transform class for object, which includes position, rotation, scale, and transform matrix.

*/
/******************************************************************************/
#ifndef RS_TRANSFORM_H_
#define RS_TRANSFORM_H_

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"

/**
 * @brief GUI Handler namespace
 */
namespace RS_Handler
{
	class RSImguiHandler;
}

/**
 * @brief Object Component namespace
*/
namespace RS_Component
{
	/**
	* @brief Transform class
	*/
	class RSTransform 
	{
	public:
		RSTransform();

		RSTransform(const glm::vec3& position_, const glm::vec3& rotation_, const glm::vec3& scale_);

		RSTransform(const RSTransform& other_);

		~RSTransform() = default;


		void Initialize() {}

		/**
		 * @brief Set position of the object
		 * @param position_ position of the object
		 */
		void SetPosition(const glm::vec3& position_);

		/**
		 * @brief Set position of the object
		 * @param x x position of the object
		 * @param y y position of the object
		 * @param z z position of the object
		 */
		void SetPosition(const float x, const float y, const float z);

		/**
		 * @brief Set rotation of the object
		 * Modulate the rotation value to 0 ~ 360
		 * @param rotation_ rotation of the object
		 */
		void SetRotation(const glm::vec3& rotation_);

		/**
		 * @brief Set rotation of the object
		 * Modulate the rotation value to 0 ~ 360
		 * @param x x rotation of the object
		 * @param y y rotation of the object
		 * @param z z rotation of the object
		 */
		void SetRotation(const float x, const float y, const float z);

		/**
		 * @brief Set scale of the object
		 * @param scale_ scale of the object
		 */
		void SetScale(const glm::vec3& scale_);

		/**
		 * @brief Set scale of the object with same scale value
		 * @param scale_ scale of the object
		 */
		void SetScale(const float scale_);

		/**
		 * @brief Set scale of the object
		 * @param x x scale of the object
		 * @param y y scale of the object
		 * @param z z scale of the object
		 */
		void SetScale(const float x, const float y, const float z);

		/**
		 * @brief Add position of the object
		 * @param position_ position of the object
		 */
		void AddPosition(const glm::vec3& position_);

		/**
		 * @brief Add position of the object
		 * @param x x position of the object
		 * @param y y position of the object
		 * @param z z position of the object
		 */
		void AddPosition(const float x, const float y, const float z);

		/**
		 * @brief Add rotation of the object
		 * @param rotation_ rotation quaternion of the object
		 */
		void AddRotation(const glm::quat& rotation_);

		/**
		 * @brief Add rotation of the object.
		 * It will convert the rotation value to quaternion and multiply it with the current rotation value.
		 * @param rotation_ rotation of the object
		 */
		void AddRotation(const glm::vec3& rotation_);

		/**
		 * @brief Add scale of the object. Not multiplied, just added.
		 * @param scale_ scale of the object
		 */
		void AddScale(const glm::vec3& scale_);

		/**
		 * @brief Add scale of the object. Not multiplied, just added.
		 * @param x x scale of the object
		 * @param y y scale of the object
		 * @param z z scale of the object
		 */
		void AddScale(const float x, const float y, const float z);

		/**
		 * @brief Get position of the object
		 * @return position of the object
		 */
		glm::vec3 GetPosition() const;

		/**
		 * @brief Get rotation of the object
		 * @return rotation of the object
		 */
		glm::vec3 GetRotation() const;

		/**
		 * @brief Get scale of the object
		 * @return scale of the object
		 */
		glm::vec3 GetScale() const;

		/**
		 * @brief Get transform matrix of the object
		 * transform matrix = translation * rotation * scale
		 * @return transform matrix of the object
		 */
		glm::mat4 GetTransformMatrix(); 


    /**
     * @brief Get transform matrix of the object without scale
     * @return transform matrix of the object without scale
     */
    glm::mat4 GetTransformMatrixWithoutScale();

    /**
     * @brief Update transform matrix of the object
		 */
		void UpdateTransformMatrix();


    /**
     * @brief dirty flag for transform matrix from GUI
		 */
		void UpdatedTransformFromGui() { b_dirty = true; }


    /**
     * @brief Reset the transform matrix to identity matrix
		 */
		void SetOrigin();

	protected:
    glm::vec3 m_position; ///< Position of the object
    glm::vec3 m_rotation; ///< Rotation of the object
    glm::vec3 m_scale; ///< Scale of the object
    glm::mat4 m_transform; ///< Transform matrix of the object
	private:
    friend class RS_Handler::RSImguiHandler; ///< Friend class for GUI handler

    bool b_dirty = true; ///< Dirty flag for transform matrix from GUI


	};
}

#endif // !RS_TRANSFORM_H_