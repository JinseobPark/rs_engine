/******************************************************************************/
/*!
\file   RSCameraBase.h
\author Jinseob Park
\date   2024/08/01

This file contains Camera Base class for Camera

*/
/******************************************************************************/
#ifndef RS_CAMERA_BASE_H_
#define RS_CAMERA_BASE_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>


#define CAMERA_MIN_NEAR 0.01f
#define CAMERA_MAX_FAR  100000.0f
constexpr int TARGET_SIZE_COUNT = 6;

/**
* @brief Camera View Type Enum
*/
enum class RSCameraType : std::uint8_t
{
	ORTHOGRAPHIC = 0,
	PERSPECTIVE
};

/**
* @brief Camera View Type Enum
*/
enum class ViewPointType : std::uint8_t
{
	FRONT = 0,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
	ISO,
};

/**
 * @brief Camera namespace
 */
namespace RS_Camera
{
	/**
	 * @brief Camera Base class
	 */
	class RSCameraBase
	{
	public:
		RSCameraBase() = default;
		explicit RSCameraBase(const glm::vec3 pos_) : position_(pos_) {}
		virtual ~RSCameraBase() = default;

		/**
		 * @brief Initialize Camera
		 */
		virtual void Initialize() = 0;

		/**
		 * @brief Get camera position
		 * @return position of camera (vec3)
		 */
		RS_N_D glm::vec3 GetPosition() const { return position_; }

		/**
		 * @brief Set camera position
		 * @param pos position of camera (vec3)
		 */
		void SetPosition(const glm::vec3 pos) { position_ = pos; }

		/**
		 * @brief Get projection matrix
		 * @return projection matrix (mat4)
		 */
		RS_N_D glm::mat4 GetProj() const { return proj_matrix_; }

		/**
		 * @brief Get view matrix
		 * @return view matrix (mat4)
		 */
		RS_N_D glm::mat4 GetView() const { return view_matrix_; }

		/**
		 * @brief Check camera view type is perspective
		 * @return true if camera view type is perspective
		 */
		RS_N_D bool GetCameraIsPerspective() const { return view_type_ == RSCameraType::PERSPECTIVE; }

		/**
		 * @brief Check camera view type is orthographic
		 * @return true if camera view type is orthographic
		 */
		RS_N_D bool GetCameraIsOrthographic() const { return view_type_ == RSCameraType::ORTHOGRAPHIC; }
	protected:
    glm::vec3 position_ = glm::vec3(0, 0, 1); ///< camera position

    glm::mat4 view_matrix_ = glm::mat4(1.0f); ///< view matrix
    glm::mat4 proj_matrix_ = glm::mat4(1.0f); ///< projection matrix
    RSCameraType view_type_ = RSCameraType::PERSPECTIVE; ///< camera view type
	};
}

#endif // !RS_CAMERA_BASE_H_