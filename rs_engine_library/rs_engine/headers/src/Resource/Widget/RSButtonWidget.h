/******************************************************************************/
/*!
\file   RSButtonWidget.h
\author Jinseob Park
\date   2024/08/01

This file contains button widget class.
This class is derived from widget base class.
This Widget include default texture, function texture, and callback function.

*/
/******************************************************************************/
#ifndef RS_BUTTON_WIDGET_H_
#define RS_BUTTON_WIDGET_H_


#include <functional>
#include "RSWidgetBase.h"

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	/**
	 * @brief Button widget class
	 * This class derived from RSWidgetBase class.
	 */
	class RSButtonWidget : public RSWidgetBase
	{
	public:
		/**
		 * @brief Constructor. Do Nothing.
		 */
		RSButtonWidget() = default;
		/**
		 * @brief Destructor. Release callback function.
		 */
		~RSButtonWidget() override;

		/**
		 * @brief Initialize button widget.
		 * It includes base widget initialize and set default texture and function texture.
		 */
		void Initialize() override;

		/**
		 * @brief Update button widget.
		 * It includes base widget update and check hover then set texture.
		 * @param dt Delta time.
		 */
		void Update(float dt) override;

		/**
		 * @brief Draw button widget.
		 * Do nothing.
		 */
		void Draw() override;

		/**
		 * @brief Destroy button widget.
		 * Do nothing.
		 */
		void Destroy() override;

		/**
		 * @brief Clicked button widget.
		 * If callback function is exist, call callback function.
		 */
		void Clicked() const;

		/**
		 * @brief Set callback function.
		 * @param callback Callback function.
		 */
		void SetCallback(const std::function<void()>& callback) { m_callback = callback; }

		/**
		 * @brief Set default texture.
		 * @param texture_id_ Texture id.
		 */
		virtual void SetDefaultTexture(const GLuint texture_id_) { m_default_texture_id = texture_id_; }

		/**
		 * @brief Get default texture.
		 * @return Default texture id.
		 */
		GLuint GetDefaultTexture() const { return m_default_texture_id; }

		/**
		 * @brief Set function texture.
		 * The function texture is used when button is hovered.
		 * @param texture_id_ Texture id.
		 */
		virtual void SetFuncTexture(const GLuint texture_id_) { m_func_texture_id = texture_id_; }

		/**
		 * @brief Get function texture.
		 * @return Function texture id.
		 */
		GLuint GetFuncTexture() const { return m_func_texture_id; }

		/**
		 * @brief Released button widget from released mouse.
		 * Do nothing.
		 */
		virtual void Released();

	protected:
    GLuint m_default_texture_id = 0; ///< Default texture id
    GLuint m_func_texture_id = 0; ///< Function texture id

    bool is_hovered = false; ///< Is hovered

		// Callback function with no arguments
    std::function<void()> m_callback; ///< Callback function

	};

}

#endif // !RS_BUTTON_WIDGET_H_