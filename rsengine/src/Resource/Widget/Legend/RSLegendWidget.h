/******************************************************************************/
/*!
\file   RSLegendWidget.h
\author Jinseob Park
\date   2024/08/01

This file contains legend widget class for SPH legend system, 
	which includes divided value, vertical, and legend image

*/
/******************************************************************************/
#ifndef RS_LEGEND_WIDGET_H_
#define RS_LEGEND_WIDGET_H_

#include "../RSButtonWidget.h"

#define LEGEND_WIDGET_NAME "LegendWidget"

/**
 * @brief Widget namespace
 */
namespace RS_Widget
{
	/**
	 * @brief Legend widget class
	 */
	class RSLegendWidget : public RSButtonWidget
	{
	public:
		/**
		 * @brief Constructor. Nothing
		 */
		RSLegendWidget();
		/**
		 * @brief Destructor. Nothing
		 */
		~RSLegendWidget() override;

		/**
		 * @brief Initialize Legend widget
		 * Set texture to legend texture.
		 * Set callback function to SetMovable
		 * Set legend width and height from texture data
		 * @see SetMovable
		 */
		void Initialize() override;
		/**
		 * @brief Nothing
		 */
		void PostInitialize() override;

		/**
		 * @brief Update Legend widget
		 * When legend widget is movable (drag), move legend widget
		 * @param dt delta time
		 * @see MoveLegendWidget
		 */
		void Update(float dt) override;

		/**
		 * @brief Draw Legend widget to visible texture buffer
		 */
		void DrawVisible() override;

		/**
		 * @brief Draw Legend widget to hittable texture buffer
		 */
		void DrawHittable() override;

		/**
		 * @brief Nothing
		 */
		void Destroy() override;

		/**
		 * @brief Create divided texture, Bind the texture to divided texture
		 * Create divided texture by divided value
		 * This function is called when divided value is changed
		 * @param divided_value_ divided value. If divided value is 0, then Bind origin texture
		 */
		void CreateDividedTexture(unsigned int divided_value_);
		
		/**
		 * @brief Set divided value. 
		 * Call CreateDividedTexture function by divided value
		 * @param divided_value_ divided value
		 */
		void SetDividedValue(unsigned int divided_value_);

		/**
		 * @brief Get divided value
		 * @return divided value
		 */
		RS_N_D unsigned int GetDividedValue() const { return divided_value; }

		/**
		 * @brief Move legend widget to mouse position
		 */
		void MoveLegendWidget();

		/**
		 * @brief Set vertical
		 * @param is_vertical_ vertical
		 */
		void SetVertical(bool is_vertical_);

		/**
		 * @brief Get vertical
		 * @return vertical
		 */
		RS_N_D bool GetVertical() const { return b_is_vertical; }

		/**
		 * @brief Switch the image between vertical and horizontal
		 * @see CreateDividedTexture
		 * @see TransposeSize
		 */
		void SwitchVerticalImage() const;

		/**
		 * @brief Released legend widget when mouse released
		 * Set movable to false
		 */
		void Released() override;

		/**
		 * @brief Set movable to true
		 */
		void SetMovable();


    /**
     * @brief Get the origin texture
     * @return origin texture
     */
    unsigned int GetOriginTexture() const { return m_origin_texture; }

    /**
    * @brief Get the divided texture
    * @return divided texture
    */
    unsigned int GetDividedTexture() const { return m_divided_texture; }

    /**
     * @brief Set the origin texture
     * @param origin_texture_ origin texture
     */
    void SetOriginTexture(const unsigned int origin_texture_) { m_origin_texture = origin_texture_; }

    /**
     * @brief Set the divided texture
     * @param divided_texture_ divided texture
     */
    void SetDividedTexture(const unsigned int divided_texture_) { m_divided_texture = divided_texture_; }

    /**
     * @brief Set the texture
     * @details Set the texture origin texture and divided texture with legend size
     * @param texture_id_ texture id
     */
		void SetTexture(const unsigned int texture_id_) override;

    /**
     * @brief Set the texture size
     * @details Get the legend texture size from texture data
     */
		void SetTextureSize();

	private:
    unsigned int divided_value = 0; ///< Divided value
		//bool  b_is_legend_image_loaded = false;
    bool b_is_vertical = true; ///< Vertical or horizontal
    unsigned int m_origin_texture = 0; ///< Origin texture
    unsigned int m_divided_texture = 0; ///< Divided texture

    int m_legend_width = 128; ///< Legend width
    int m_legend_height = 512; ///< Legend height

    bool b_movable = false; ///< Movable or not
	};
}

#endif // !RS_LEGEND_WIDGET_H_