/******************************************************************************/
/*!
\file   RSDirectionView.h
\author Jinseob Park
\date   2024/08/01

This file contains direction view class for screen space widget.

*/
/******************************************************************************/
#ifndef RS_DIRECTION_VIEW_H_
#define RS_DIRECTION_VIEW_H_

#include "../RSMeshWidget.h"
#include "Resource/Object/Camera/RSCamera.h"

/**
 * @brief Camera namespace
 */
namespace RS_Camera
{
	class RSCamera;
}

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	/**
	 * @brief Direction View Class.
	 */
	class RSDirectionView
	{
	public:
		/**
		 * @brief Constructor.
		 * Allocate memory for each axis widget.
		 */
		RSDirectionView();
		~RSDirectionView();

		/**
		 * @brief Initialize Direction View.
		 * Set mesh data, color, size, position, and add widget to widget manager.
		 */
		void Initialize();
		void PostInitialize();
		/**
		 * @brief Update Direction View.
		 * @param dt delta time.
		 */
		void Update(float dt) const;
		void Draw();

		/**
		 * @brief Deallocate memory for each axis widget.
		 */
		void Shutdown() const;

		/**
		 * @brief Set Direction View Visible.
		 * @param is_visible_ true if visible, false if invisible.
		 */
		void SetDirectionViewVisible(bool is_visible_);

		/**
		 * @brief Get Direction View Visible.
		 * @return true if visible, false if invisible.
		 */
		RS_N_D bool GetDirectionViewVisible() const { return m_is_visible; }

	private:
    bool m_is_visible = true; ///< Is visible or not
    RS_Camera::RSCamera* p_camera = nullptr; ///< Camera pointer

    RSMeshWidget* m_x_axis_widget = nullptr; ///< X axis widget
    RSMeshWidget* m_y_axis_widget = nullptr; ///< Y axis widget
    RSMeshWidget* m_z_axis_widget = nullptr; ///< Z axis widget
    RSMeshWidget* m_center_widget = nullptr; ///< Center widget
	};
}

#endif // !RS_DIRECTION_VIEW_H_