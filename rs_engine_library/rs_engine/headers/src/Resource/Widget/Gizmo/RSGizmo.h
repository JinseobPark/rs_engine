/******************************************************************************/
/*!
\file   RSGizmo.h
\author Jinseob Park
\date   2024/08/01

This file contains gizmo system for object, which includes translation, rotation, and scale.

*/
/******************************************************************************/
#ifndef RS_GIZMO_H_
#define RS_GIZMO_H_

#include "RSGizmoWidget.h"
#include "Resource/Object/RSObject.h"
//#include "Define/RSWidgetData.h"

/**
 * @brief Object namespace
 */
namespace RS_Object
{
	class RSObject;
}

///**
// * @brief Gizmo state enum class
// */
//enum class RSGizmoState : std::uint8_t
//{
//	NONE = 0,
//	TRANSLATION,
//	ROTATION,
//	SCALE
//};

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	/**
	 * @brief Gizmo class
	 */
	class RSGizmo
	{
	public:

		/**
		 * @brief Allocate memory for gizmo widgets
		 */
		RSGizmo();
		~RSGizmo();

		/**
		 * @brief Initialize gizmo widgets
		 * Load Meshes, set color, set size, set position
		 */
		void Initialize() const;

		/**
		 * @brief Update gizmo widgets
		 * Gizmo state is checked and updated
		 * TODO : Implement gizmo update function
		 * @param dt Delta time
		 * @see SetObjectPositionByGizmo
		 */
		void Update(float dt);

		void Draw();
		//void DrawHittable();
		//void DrawVisible();

		void Destroy();

		/**
		 * @brief Set target object
		 * 
		 * @param object object to be manipulated by gizmo
		 */
		void SetTargetObject(RS_Object::RSObject* object);

		/**
		 * @brief Set gizmo state
		 *
		 * @param state gizmo state
		 */
		void SetState(RSGizmoState state);

		/**
		 * @brief Get gizmo state
		 *
		 * @return RSGizmoState
		 */
		RSGizmoState GetState() const { return m_state; }

		/**
		 * @brief Set start mouse position
		 *
		 * @param mouse_x mouse x position
		 * @param mouse_y mouse y position
		 */
		void SetStartMousePos(UINT mouse_x, UINT mouse_y);

		/**
		 * @brief Set activated Gizmo by id.
		 * 
		 * @param gizmo_id gizmo id.
		 */
		void SetActivatedGizmoID(const UINT gizmo_id) { m_activated_gizmo_id = gizmo_id; }

		/**
		 * @brief Get activated gizmo id.
		 * 
		 * @return UINT activated gizmo id.
		 */
		UINT GetActivatedGizmoID() const { return m_activated_gizmo_id; }

		/**
		 * @brief Set Object position by gizmo.
		 * 
		 * @param x_ x position
		 * @param y_ y position
		 * @param z_ z position
		 */
		void SetObjectPositionByGizmo(bool x_, bool y_, bool z_);

		//void SetObjectPositionByGizmo2(bool x_, bool y_, bool z_) const;

		/**
		 * @brief Set Object rotation by gizmo.
		 *
		 * @param x_ x rotation
		 * @param y_ y rotation
		 * @param z_ z rotation
		 */
		void SetObjectRotationByGizmo(bool x_, bool y_, bool z_);

		/**
		 * @brief Set Object scale by gizmo.
		 *
		 * @param x_ x scale
		 * @param y_ y scale
		 * @param z_ z scale
		 */
		void SetObjectScaleByGizmo(bool x_, bool y_, bool z_);

		/**
		 * @brief Set gizmo state to NONE
		 */
		void SetStateResetGizmo() const;
		
	private:

    /**
     * @brief Calculate ray origin and direction from mouse position
     * @param [out] ray_origin_out get ray origin point
     * @param [out] ray_dir_out get ray direction vector
     * @param [in] mouse_pos mouse position
		 */
		void CalculateRayOriginDirection(glm::vec3& ray_origin_out, glm::vec3& ray_dir_out, const glm::vec2& mouse_pos) const;


		/**
		 * @brief Get ray origin and direction from screen position
		 * @param ray_origin_out get ray origin point
		 * @param ray_direction_out get ray direction vector
		 * @param screen_pos screen position
		 */
		void GetRayScreen(glm::vec3& ray_origin_out, glm::vec3& ray_direction_out, const glm::vec2& screen_pos);

	protected:
    RSGizmoState m_state = RSGizmoState::NONE; ///< Gizmo state

    RS_Object::RSObject* target_object = nullptr; ///< Target object

    glm::vec2 start_mouse_pos = glm::vec2(0, 0); ///< Start mouse position
    RS_Component::RSTransform start_object_model_matrix = RS_Component::RSTransform(); ///< Start object model matrix

    UINT m_activated_gizmo_id = 0; ///< Activated gizmo id


		// RSGizmo data. Delete function is called in WidgetManager
    RSGizmoWidget* m_translate_x; ///< Translate x gizmo widget
    RSGizmoWidget* m_translate_y; ///< Translate y gizmo widget
    RSGizmoWidget* m_translate_z; ///< Translate z gizmo widget
    RSGizmoWidget* m_translate_center; ///< Translate center gizmo widget
    RSGizmoWidget* m_translate_yz; ///< Translate yz gizmo widget
    RSGizmoWidget* m_translate_xz; ///< Translate xz gizmo widget
    RSGizmoWidget* m_translate_xy; ///< Translate xy gizmo widget
    RSGizmoWidget* m_rotate_x; ///< Rotate x gizmo widget
    RSGizmoWidget* m_rotate_y; ///< Rotate y gizmo widget
    RSGizmoWidget* m_rotate_z; ///< Rotate z gizmo widget
    RSGizmoWidget* m_scale_x; ///< Scale x gizmo widget
    RSGizmoWidget* m_scale_y; ///< Scale y gizmo widget
    RSGizmoWidget* m_scale_z; ///< Scale z gizmo widget
    RSGizmoWidget* m_scale_center; ///< Scale center gizmo widget
							  
	};
}

#endif // !RS_GIZMO_H_