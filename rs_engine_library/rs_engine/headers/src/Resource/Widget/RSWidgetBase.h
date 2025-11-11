/******************************************************************************/
/*!
\file   RSWidgetBase.h
\author Jinseob Park
\date   2024/08/01

This file contains widget base class for widget
This widget includes widget id, widget name, and widget visibility.

*/
/******************************************************************************/
#ifndef RS_WIDGET_BASE_H_
#define RS_WIDGET_BASE_H_

#include "glm/glm.hpp"
#include "Resource/Object/Component/RSTransform.h"
#include "Define/RSWidgetData.h"

namespace RS_Component
{
	class RSTransform;
}

///**
// * @brief Widget Visibility
// * It includes hit visible, none hit visible, hit invisible, none hit invisible
// */
//enum class RSVisibility : std::uint8_t
//{
//	HIT_VISIBLE = 0,
//	NONE_HIT_VISIBLE,
//	HIT_INVISIBLE,
//	NONE_HIT_INVISIBLE
//};

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	/**
	 * @brief Widget base class
	 * This class includes widget id, widget name, widget visibility, widget position, widget size, widget rotation, widget color, widget texture.
	 */
	class RSWidgetBase
	{
	public:
		/**
		 * @brief Constructor. Do nothing.
		 */
		RSWidgetBase();
		virtual ~RSWidgetBase() = default;

		/**
		 * @brief Initialize widget.
		 * If texture id is 0, set default texture.
		 */
		virtual void Initialize();
		/**
		 * @brief Post initialize widget. Do nothing.
		 */
		virtual void PostInitialize();

		/**
		 * @brief Update widget. Do nothing.
		 * @param dt Delta time
		 */
		virtual void Update(float dt);

		/**
		 * @brief Draw widget. Do nothing.
		 */
		virtual void Draw();

		/**
		 * @brief Draw visible widget.
		 */
		virtual void DrawVisible();

		/**
		 * @brief Draw hittable widget for picking.
		 */
		virtual void DrawHittable();

		/**
		 * @brief Destroy widget. Do nothing.
		 */
		virtual void Destroy();

		/**
		 * @brief Set Widget ID. This ID is also used for picking.
		 * ID is unique for each widget.
		 * @param id Widget ID
		 */
		void SetWidgetID(const unsigned int id) { widget_id = id; }

		/**
		 * @brief Get Widget ID.
		 * @return Widget ID
		 */
		unsigned int GetWidgetID() const { return widget_id; }

		/**
		 * @brief Set Widget Visibility.
		 * @param visibility_ Widget Visibility
		 */
		void SetVisibility(const RSVisibility visibility_) { m_visibility = visibility_; }

		/**
		 * @brief Get Widget Visibility.
		 * @return Widget Visibility
		 */
		RSVisibility GetVisibility() const { return m_visibility; }

		/**
		 * @brief Set Widget Name for finding widget.
		 * @param name_ Widget Name
		 */
		void SetWidgetName(const std::string& name_) { widget_name = name_; }

		/**
		 * @brief Get Widget Name.
		 * @return Widget Name
		 */
		std::string GetWidgetName() { return widget_name; }

		/**
		 * @brief Set Z Order. Higher z order, upper widget.
		 * @param z_order_ Z Order
		 */
		void SetZOrder(const unsigned int z_order_) { z_order = z_order_; }

		/**
		 * @brief Get Z Order.
		 * @return Z Order
		 */
		unsigned int GetZOrder() const { return z_order; }

		/**
		 * @brief Set Texture ID.
		 * @param texture_id Texture ID
		 */
		virtual void SetTexture(const GLuint texture_id) { m_texture_id = texture_id; }

		/**
		 * @brief Get Texture ID.
		 * @return Texture ID
		 */
		GLuint GetTexture() const { return m_texture_id; }

		/**
		 * @brief Set Position
		 * @param pos_x	x position
		 * @param pos_y y position
		 */
		virtual void SetPosition(const float pos_x, const float pos_y) { m_transform.SetPosition(glm::vec3(pos_x, pos_y, 0.0f)); }

		/**
		 * @brief Set Position
		 * @param pos_ position
		 */
		void SetPosition(const glm::vec3 pos_) { m_transform.SetPosition(pos_); }

		/**
		 * @brief Add Position
		 * @param pos_x x position
		 * @param pos_y y position
		 */
		void AddPosition(const float pos_x, const float pos_y) { m_transform.AddPosition(glm::vec3(pos_x, pos_y, 0.0f)); }

		/**
		 * @brief Get Position
		 * @return Position
		 */
		glm::vec3 GetPosition() const { return m_transform.GetPosition(); }

		/**
		 * @brief Set Size
		 * @param width width
		 * @param height height
		 * @param depth depth
		 */
		void SetSize(const float width, const float height, const float depth = 1.0f) { m_transform.SetScale(glm::vec3(width, height, depth)); }

		/**
		 * @brief Set Size 2D
		 * @param width width
		 * @param height height
		 */
		void SetSize2D(const float width, const float height) { m_transform.SetScale(glm::vec3(width, height, 1.0f)); }

		/**
		 * @brief Set Size 2D
		 * @param size_ size
		 */
		void SetSize2D(const float size_) { m_transform.SetScale(glm::vec3(size_, size_, 1.0f)); }

		/**
		 * @brief Set Size 3D
		 * @param width width
		 * @param height height
		 * @param depth depth
		 */
		void SetSize3D(const float width, const float height, const float depth) { m_transform.SetScale(glm::vec3(width, height, depth)); }

		/**
		 * @brief Set Size 3D
		 * @param size_ size
		 */
		void SetSize3D(const float size_) { m_transform.SetScale(glm::vec3(size_, size_, size_)); }

		/**
		 * @brief Transpose Y and X
		 */
		void TransposeSize();

		/**
		 * @brief Get Size
		 * @return Size
		 */
		glm::vec3 GetSize() const { return m_transform.GetScale(); }

		/**
		 * @brief Set Rotation
		 * @param rot_x x rotation
		 * @param rot_y y rotation
		 * @param rot_z z rotation
		 */
		void SetRotation(const float rot_x, const float rot_y, const float rot_z) { m_transform.SetRotation(glm::vec3(rot_x, rot_y, rot_z)); }

		/**
		 * @brief Get Rotation
		 * @return Rotation
		 */
		glm::vec3 GetRotation() const { return m_transform.GetRotation(); }

		/**
		 * @brief Add Rotation
		 * @param rot_x x rotation
		 * @param rot_y y rotation
		 * @param rot_z z rotation
		 */
		void AddRotation(const float rot_x, const float rot_y, const float rot_z) { m_transform.AddRotation(glm::vec3(rot_x, rot_y, rot_z)); }

		/**
		 * @brief Set Color
		 * @param r red
		 * @param g green
		 * @param b blue
		 * @param a alpha
		 */
		void SetColor(const float r, const float g, const float b, const float a = 1.0f) { m_color = glm::vec4(r, g, b, a); }

		/**
		 * @brief Get Color
		 * @return Color
		 */
		void SetColor(const glm::vec4 color) { m_color = color; }

		/**
		 * @brief Get Color
		 * @return Color
		 */
		glm::vec4 GetColor() const { return m_color; }

		/**
		 * @brief Set Alpha
		 * @param alpha alpha
		 */
		void SetAlpha(const float alpha) { m_alpha = alpha; m_color.w = alpha; }

		/**
		 * @brief Get Alpha
		 * @return Alpha
		 */
		float GetAlpha() const { return m_alpha; }

		/**
		 * @brief Get Model transform matrix
		 * @return Model transform matrix
		 */
		glm::mat4 GetModelMatrix() const;

	protected:
    RS_Component::RSTransform m_transform; ///< Transform
    float m_alpha = 1.0f; ///< Alpha
    int b_flip_y = 1; ///< Flip Y
    glm::vec4 m_color = glm::vec4(1.0f); ///< Color
    GLuint m_texture_id = 0; ///< Texture ID
    unsigned int widget_id = 0; ///< Widget ID
		unsigned int z_order = 0; // higher z_order, upper widget
    std::string widget_name; ///< Widget Name
    RSVisibility m_visibility = RSVisibility::HIT_VISIBLE; ///< Visibility


	};
}

#endif // !RS_WIDGET_BASE_H_