/******************************************************************************/
/*!
\file   RSTextWidget.h
\author Jinseob Park
\date   2024/08/01

This file contains text widget class for rendering text on the screen.
This file include freetype library for rendering text.
This widget class is derived from widget base class.
This widget include text size, text color, and text position.

*/
/******************************************************************************/
#ifndef RS_TEXT_WIDGET_H_
#define RS_TEXT_WIDGET_H_

#include "RSWidgetBase.h"
#include "Define/RSWidgetData.h"
#include <freetype/ft2build.h>
#include FT_FREETYPE_H

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	///**
	// * @brief Text static dynamic enum class
	// * Static - text is not changed dynamically
	// * Dynamic - text is changed dynamically
	// * When text isn't changed dynamically, please use static
	// */
	//enum class RSTextStaticDynamic : std::uint8_t
	//{
	//	STATIC = 0,
	//	DYNAMIC
	//};

	/**
	 * @brief Text widget class
	 */
	class RSTextWidget : public RSWidgetBase
	{
	public:
		/**
		 * @brief Constructor
		 * Commonly, text type is static
		 * Visible is NONE_HIT_VISIBLE
		 * @param type_ - text type
		 */
		explicit RSTextWidget(RSTextStaticDynamic type_ = RSTextStaticDynamic::STATIC);
		/**
		 * @brief Destructor. Nothing to do
		 */
		~RSTextWidget() override;

		/**
		 * @brief Initialize text widget
		 * If text type is dynamic, create text VAO, VBO
		 */
		void Initialize() override;

		/**
		 * @brief Shutdown text widget
		 * Release text texture, text VAO, VBO
		 */
		void Shutdown();

		/**
		 * @brief Update text widget
		 * If text is changed, render text to texture
		 * @param dt Delta time
		 * @see RenderTextToTexture
		 */
		void Update(float dt) override;

		/**
		 * @brief Draw text widget. Do nothing
		 * This function was substituted by DrawVisible on widget base class
		 */
		void Draw() override;

		/**
		 * @brief Create text texture
		 * Draw text to other frame buffer and capture texture
		 * If type is static, text texture is created only once
		 */
		void RenderTextToTexture();

		/**
		 * @brief Set text and dirty flag
		 * @param text_ Text to draw
		 */
		void SetText(const std::wstring& text_);

		/**
		 * @brief Get text
		 * @return Text (wstring)
		 */
		std::wstring GetText() const { return m_text; }

		/**
		 * @brief Set text size like font size
		 * @param size_ Text size
		 */
		void SetTextSize(float size_);

		/**
		 * @brief Get text size
		 * @return Text size
		 */
		float GetTextSize() const { return text_size; }

		/**
		 * @brief Set text position
		 * @param pos_ Text position
		 */
		void SetPosition(glm::vec2 pos_);

		/**
		 * @brief Set text position
		 * @param pos_x Text position x
		 * @param pos_y Text position y
		 */
		void SetPosition(float  pos_x, float pos_y) override;

		/**
		 * @brief Get text VAO
		 * @return Text VAO
		 */
		unsigned int GetTextVao() const { return text_vao_; }

		/**
		 * @brief Set Text color
		 * @param color_ Text color (vec3)
		 */
		void SetTextColor(const glm::vec3 color_) { m_text_color = color_; }
		/**
		 * @brief Get Text color
		 * @return Text color (vec3)
		 */
		glm::vec3 GetTextColor() const { return m_text_color; }

		/**
		 * @brief Call when window is resized for resizing text texture
		 */
		void Resize();

	private:
    RSTextStaticDynamic text_type_ = RSTextStaticDynamic::STATIC; ///< Text type
    bool dirty_ = false; ///< Dirty flag
		//unsigned int text_texture_ = 0;
    float text_size = 0.3f; ///< Text size
    glm::vec3 origin_position = glm::vec3(0.0f); ///< Origin position
    glm::vec3 m_text_color = glm::vec3(0.0f); ///< Text color
		
    std::wstring m_text; ///< Text

    unsigned int text_vao_ = 0, text_vbo_ = 0; ///< Text VAO, VBO
    unsigned int text_texture_vao = 0, text_texture_vbo = 0; ///< Text texture VAO, VBO

    float texture_width = 0, texture_height = 0; ///< Texture width, height
	};
}

#endif // !RS_TEXT_WIDGET_H_