/*****************************************************************//**
 * \file   RSTextButtonWidget.cpp
 * \brief  RS Engine text button widget.
 * This class is derived from button widget class.
 * This widget include text size, text color and text position.
 * This widget include text alignment of the texture widget.
 *
 * \author Jinseob Park
 * \date   2025/02/18
 *********************************************************************/
#ifndef RS_TEXT_BUTTON_WIDGET_H_
#define RS_TEXT_BUTTON_WIDGET_H_

#include "RSButtonWidget.h"
#include "Define/RSWidgetData.h"
//#include <freetype/freetype.h>


 /**
  * @brief Widget namespace
  */
namespace RS_Widget
{
  class RSTextButtonWidget : public RSButtonWidget
  {
  public:
    RSTextButtonWidget(const std::wstring& text_);

		~RSTextButtonWidget() override;


		/**
		 * @brief Initialize text widget
		 * @see RenderTextToTexture
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
		 */
		void Update(float dt) override;

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
		float GetTextSize() const { return m_text_size; }

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
		 * @brief Set Text color
		 * @param color_ Text color (vec3)
		 */
		void SetTextColor(const glm::vec3 color_) { m_text_color = color_; }
		/**
		 * @brief Get Text color
		 * @return Text color (vec3)
		 */
		glm::vec3 GetTextColor() const { return m_text_color; }

		void SetTextAlignment(RSTextAlignment text_align_);

		RSTextAlignment GetTextAlignment() const { return m_text_alignment; }

		/**
		 * @brief Call when window is resized for resizing text texture
		 */
		void Resize();

    /**
     * @brief Set default texture
     * @param texture_id_ Texture id
		 */
		void SetDefaultTexture(const GLuint texture_id_) override;

    /**
     * @brief Set function texture
     * @param texture_id_ Texture id
		 */
		void SetFuncTexture(const GLuint texture_id_) override;

  private:
		/**
		 * @brief Get text VAO
		 * @return Text VAO
		 */
		unsigned int GetTextVao() const { return text_vao_; }

    /**
     * @brief Get text VBO
		 */
		void DeleteCustomTextures();

    /**
     * @brief Create text buffer
		 */
		void CreateTextBuffer();

    /**
     * @brief Delete text buffer
		 */
		void DeleteTextBuffer();

    /**
     * @brief Create text texture
		 */
		void CreateTextTexture();

    /**
     * @brief Draw text texture
		 */
		void DrawDefaultTexture();

    /**
     * @brief Draw text texture
     */
    void DrawFuncTexture();

    /**
     * @brief Draw text texture
		 */
		void DrawTextTexture();

    /**
     * @brief Create default texture
		 */
		void CreateDefaultTexture();

    /**
     * @brief Create function texture
     */
    void CreateFuncTexture();

    /**
     * @brief Calculate text position
		 */
		void CalculateTextPosition();

  private:
    RSTextAlignment m_text_alignment = RSTextAlignment::MIDDLE; ///< Text alignment
    bool m_dirty = false; ///< Dirty flag
		//unsigned int text_texture_ = 0;
    float m_text_size = 0.3f; ///< Text size
    glm::vec3 m_origin_position = glm::vec3(0.0f); ///< Origin position
    glm::vec3 m_text_color = glm::vec3(0.0f); ///< Text color

    glm::vec3 m_text_position = glm::vec3(0.0f); ///< Text position

    GLuint m_default_text_texture = 0; ///< Default text texture
    GLuint m_func_text_texture = 0; ///< Function text texture
    GLuint m_text_texture = 0; ///< Text texture

    std::wstring m_text; ///< Text

    unsigned int text_vao_ = 0, text_vbo_ = 0; ///< Text VAO, VBO
    unsigned int text_texture_vao = 0, text_texture_vbo = 0; ///< Text texture VAO, VBO

    float texture_width = 0, texture_height = 0; ///< Texture width, height

    float m_descent = 0.0f, m_anti_text_size = 0.0f; ///< Descent, Anti text size
  };
}


#endif
