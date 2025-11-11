/******************************************************************************/
/*!
\file   RSMeshWidget.h
\author Jinseob Park
\date   2024/08/01

This file contains mesh widget class for object, which includes mesh and widget base class.

*/
/******************************************************************************/
#ifndef RS_MESH_WIDGET_H_
#define RS_MESH_WIDGET_H_

#include "Resource/Object/Component/Mesh/RSMesh.h"
#include "RSWidgetBase.h"

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	using RS_Mesh::RSMesh;

	/**
	 * @brief Mesh widget class
	 * This class derived from RSWidgetBase class.
	 */
	class RSMeshWidget : public RSWidgetBase
	{
	public:
		/**
		 * @brief Constructor. Create mesh object.
		 */
		RSMeshWidget();
		/**
		 * @brief Destructor. Release mesh object.
		 */
		~RSMeshWidget() override;

		/**
		 * @brief Initialize mesh widget.
		 * It includes base widget initialize.
		 */
		void Initialize() override;
		void PostInitialize() override;

		/**
		 * @brief Update mesh widget.
		 * It includes base widget update.
		 * @param dt Delta time.
		 */
		void Update(float dt) override;
		void Draw() override;

		/**
		 * @brief Draw mesh widget.
		 * Draw mesh object.
		 */
		void DrawVisible() override;

		/**
		 * @brief Draw mesh widget.
		 * Do nothing.
		 */
		void DrawHittable() override;

		/**
		 * @brief Set mesh object.
		 * @param mesh Mesh object.
		 */
		void SetMesh(RSMesh* mesh) { m_mesh = mesh; }

		/**
		 * @brief Get mesh object.
		 * @return Mesh object.
		 */
		RSMesh* GetMesh() const { return m_mesh; }


    /**
     * @brief Set fixed rotation.
     * @param rotation_ Fixed rotation matrix
     */
    void SetFixedRotation(const glm::mat4& rotation_) { m_fixed_rotation = rotation_; }

	private:
    RSMesh* m_mesh = nullptr; ///< Mesh object

    glm::mat4 m_fixed_rotation = glm::mat4(1.0f); ///< Fixed rotation matrix


	};
}

#endif // !RS_MESH_WIDGET_H_