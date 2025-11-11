/**************************************************************************************************************/
/*!
\file   RSModel.h
\author Jinseob Park
\date   2024/08/01

This file contains model component for object, which includes mesh, color, transparency, and visibility.

*/
/**************************************************************************************************************/
#ifndef RS_MODEL_H_
#define RS_MODEL_H_

#include <glm/ext/matrix_transform.hpp>

#include "../Mesh/RSMesh.h"
#include "../RSComponent.h"

/**
 * @brief Imgui Handler namespace
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
	using RS_Mesh::RSMesh;

	/**
	 * @brief Model class for object component. 
	 * It derives from RSComponent.
	 */
	class RSModel : public RSComponent
	{
	public:
		RSModel() : RSComponent(RSComponentType::CT_MODEL) {}
		~RSModel() override;

    // Deep copy constructor
		RSModel(const RSModel& other);

    RSComponent* Clone() const override { return new RSModel(*this); }

		/**
		 * @brief Get name of component
		 * @return name of component. (RSModel)
		 */
		std::string GetName() override { return "RSModel"; }

		void Initialize() override {}

		/**
		 * @brief Draw Mesh model by texture
		 * Should have Mesh
		 */
		void Draw();

		/**
		 * @brief Draw Wireframe model by default texture
		 * Should have Mesh. 
		 */
		void DrawWireFrame() ;


    /**
     * @brief Draw mesh model with PBR shader
     * The owner should have Mesh and Material.
		 */
		void DrawPbr();

		/**
		 * @brief Set visible of model
		 * @param visible visibility of model
		 */
		void SetVisible(const bool visible) { m_is_visible = visible; }

		/**
		 * @brief Set color of model
		 * @param color color of model
		 */
		void SetColor(const glm::vec3& color) { m_color = color; }

		/**
		 * @brief Check visibility of model.
		 * @return visibility of model
		 */
		RS_N_D bool IsVisible() const { return m_is_visible; }

		/**
		 * @brief Check visibility of model. It Same with IsVisible()
		 * @see IsVisible()
		 * @return visibility of model
		 */
		RS_N_D bool GetIsVisible() const { return m_is_visible; }

		/**
		 * @brief Get color of model
		 * @return color of model
		 */
		RS_N_D glm::vec3 GetColor() const { return m_color; }

		/**
		 * @brief Set Mesh pointer of model
		 * @param mesh mesh pointer
		 */
		void SetMesh(RSMesh* mesh) { m_mesh = mesh; }

		/**
		 * @brief Get Mesh pointer of model
		 * @return mesh pointer
		 */
		RS_N_D RSMesh* GetMesh() const { return m_mesh; }

		/**
		 * @brief Get transparency value of model
		 */
		RS_N_D float GetTransparency() const { return m_transparency; }

		/**
		 * @brief Set transparency of model
		 * When transparency is less than 1.0f, it is not opaque object.
		 * @see SetIsOpaqueObject()
		 * @param transparency_ transparency of model
		 */
		void SetTransparency(float transparency_);

		/**
		 * @brief Set opaque object
		 * @param is_opaque_ opaque object
		 */
		void SetIsOpaqueObject(const bool is_opaque_) { is_opaque = is_opaque_; }

		/**
		 * @brief Get opaque object
		 * @return opaque object
		 */
		RS_N_D bool GetIsOpaqueObject() const { return is_opaque; }

		/**
		 * @brief Draw bounding box of model. But, It is not used currently.
		 */
		void DrawBoundingBox();


    /**
     * @brief Set Tiling of texture
     * @param tiling_ tiling of texture
		 */
		void SetTiling(const glm::vec2& tiling_);

    /**
     * @brief Set Tilling of texture
		 * @param x_ tile x
		 * @param y_ tile y
		 */
		void SetTilling(const float x_, const float y_);

    /**
     * @brief Set Offset of texture
     * @param offset_ offset of texture
		 */
		void SetOffset(const glm::vec2& offset_);

    /**
     * @brief Set Offset of texture
     * @param x_ offset x
     * @param y_ offset y
		 */
		void SetOffset(const float x_, const float y_);

    /**
     * @brief Update texture transform
		 */
		void UpdateTexTransform();

    /**
     * @brief Get Tiling of texture
     * @return tiling of texture
     */
    glm::vec2 GetTiling() const { return m_tiling; }

    /**
     * @brief Get Offset of texture
     * @return offset of texture
     */
    glm::vec2 GetOffset() const { return m_offset; }

    /**
     * @brief Get Texture transform
     * @return texture transform
		 */
		glm::mat4 GetTexTransform() const { return m_tex_transform; }

	private:
    bool m_is_visible = true; ///< Visibility of model
    glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f); ///< Color of model

    glm::vec2 m_tiling = glm::vec2(1.0f); ///< Tiling of texture
    glm::vec2 m_offset = glm::vec2(0.0f); ///< Offset of texture

    glm::mat4 m_tex_transform = glm::mat4(1.0f); ///< Texture transform

    bool is_opaque = true; ///< Opaque object
    float m_transparency = 1.0f; ///< Transparency of model
    RSMesh* m_mesh = nullptr; ///< Mesh of model
    friend class RS_Handler::RSImguiHandler; ///< Imgui handler
	};
}

#endif // !RS_MODEL_H_