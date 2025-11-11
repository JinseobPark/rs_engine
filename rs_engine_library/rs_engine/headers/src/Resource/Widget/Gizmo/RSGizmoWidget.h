/******************************************************************************/
/*!
\file   RSGizmoWidget.h
\author Jinseob Park
\date   2024/08/01

This file contains gizmo widget class for object, 
	which includes mesh, color, and draw function.

*/
/******************************************************************************/
#ifndef RS_GIZMO_WIDGET_H_
#define RS_GIZMO_WIDGET_H_

#include "../RSButtonWidget.h"
#include "Resource/Object/Component/Mesh/RSMesh.h"
//#include "functional"

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	using RS_Mesh::RSMesh;

	/**
	 * @brief Gizmo widget class
	 */
	class RSGizmoWidget : public RSButtonWidget
	{
	public:
		/**
		 * @brief Allocate Mesh and Set size to 100.
		 */
		RSGizmoWidget();

		/**
		 * @brief Delete Mesh
		 */
		~RSGizmoWidget() override;

		void Initialize() override;
		void PostInitialize() override;

		/**
		 * @brief Check is hovered. then, switch gizmo color to picking color(Yellow).
		 * 
		 * @param dt Delta time
		 */
		void Update(float dt) override;

		/**
		 * @brief Nothing. This function was substituted by DrawVisible and DrawHittable.
		 */
		virtual void Draw() override;

		/**
		 * @brief Draw Gizmo widget to Visible texture buffer
		 */
		virtual void DrawVisible() override;

		/**
		 * @brief Draw Gizmo widget to picking texture buffer
		 */
		virtual void DrawHittable() override;

		/**
		 * @brief Nothing.
		 */
		void Destroy() override;

		/**
		 * @brief Set Mesh to Gizmo widget for drawing
		 * @param mesh Mesh pointer
		 */
		void SetMesh(RSMesh* mesh) { m_mesh = mesh; }

		/**
		 * @brief Get Mesh pointer
		 * @return Mesh pointer
		 */
		RS_N_D RSMesh* GetMesh() const { return m_mesh; }

		/**
		 * @brief Set default color
		 * @param color Default color vec4(r, g, b, a)
		 */
		void SetDefaultColor(const glm::vec4 color) { default_color = color; }

		/**
		 * @brief Set default color. Alpha value is 1.0f
		 * @param r Red
		 * @param g Green
		 * @param b Blue
		 */
		void SetDefaultColor(const float r, const float g, const float b) { default_color = glm::vec4(r, g, b, 1.0f); }

		/**
		 * @brief Get default color
		 * @return Default color
		 */
		RS_N_D glm::vec4 GetDefaultColor() const { return default_color; }

		/**
		 * @brief Set selected color
		 * @param color Selected color vec4(r, g, b, a)
		 */
		void SetSelectedColor(const glm::vec4 color) { selected_color = color; }

		/**
		 * @brief Get selected color
		 * @return Selected color vec4(r, g, b, a)
		 */
		RS_N_D glm::vec4 GetSelectedColor() const { return selected_color; }

		/**
		 * @brief Set is draw
		 * Also, this function Set visible and hittable.
		 * @param is_draw_ is drawn
		 */
		void SetIsDraw(bool is_draw_);

		/**
		 * @brief Get is draw
		 * @return is drawn
		 */
		RS_N_D bool GetIsDraw() const { return is_draw; }


    /**
     * @brief Rotate the gizmo object with rotation matrix for n-axis.
     * @details All gizmo object meshes are x-axis oriented. So y, z axis objects should be rotated.
     * @param axis gizmo axis type
     */
    void SetGizmoAxisType(GizmoAxisType axis);


	protected:
    RSMesh* m_mesh = nullptr; ///< Mesh pointer
    glm::vec4 default_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); ///< Default color
    glm::vec4 selected_color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); ///< Selected color
    bool is_draw = false; ///< Is drawn

    GizmoAxisType m_axis_type = GizmoAxisType::CENTER; ///< Gizmo axis type
    glm::mat4 m_fixed_rotation = glm::mat4(1.0f); ///< Fixed rotation matrix for n-axis


	};
}

#endif // !RS_GIZMO_WIDGET_H_