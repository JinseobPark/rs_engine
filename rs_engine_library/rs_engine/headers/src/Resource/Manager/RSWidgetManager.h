/******************************************************************************/
/*!
\file   RSWidgetManager.h
\author Jinseob Park
\date   2024/08/01

This file contains Widget Manager for Widget Managing.

*/
/******************************************************************************/
#ifndef RS_WIDGET_MANAGER_H_
#define RS_WIDGET_MANAGER_H_

#include "../Widget/RSWidgetBase.h"
#include "../Widget/RSButtonWidget.h"
#include "../Widget/RSTextButtonWidget.h"
#include "../Widget/RSTextWidget.h"
#include "../Widget/Gizmo/RSGizmo.h"
#include "../Widget/DirectionView/RSDirectionView.h"
#include "../Widget/Legend/RSLegendWidget.h"

/// Gizmo ID
constexpr int GIZMO_START_ID= 10;
constexpr int GIZMO_END_ID = 33;

/// TODO : Change this to class variable member for interactive use
#define DIRECTION_VIEW_USE true
#define LEGEND_VIEW_USE true


/**
* @brief Widget namespace
*/
namespace RS_Widget
{
/**
* @brief Widget Manager
*/
class RSWidgetManager
{
public:
	RSWidgetManager();
	~RSWidgetManager();

	/**
	* @brief Initialize all widgets
	* Gizmo Create and Initialize
	* If direction view using, create and initialize direction view
	* 
	* @see InitializeQuad() (private)
	* @return void
	*/
	void Initialize();

	/**
	* @brief Sort widgets by z-index
	* @see SortWidgets()
	*/
	void PostInitialize();

	/**
	* @brief Update all widgets
	* Gizmo Update
	* If direction view using, update direction view
	* @param dt delta time
	* @return void
	* @see SortWidgets()
	*/
	void Update(float dt);

	/**
	* @brief Sort Widgets by z-order
	*/
	void SortWidgets();

	/**
	* @brief Draw widgets. 
	* Note : this function was separated by visible draw and hittable draw
	* @see VisibleDraw()
	* @see HittableDraw()
	*/
	void Draw();

	/**
	* @brief Draw visible widgets for rendering
	* @see RSVisibility
	*/
	void VisibleDraw() const;

	/**
	* @brief Draw Hittable widgets for picking
	* @see RSVisibility
	*/
	void HittableDraw() const;

	/**
	 * @brief Call function when mouse is clicked and hit any widget (not 0)
	 * @param[in] widget_id hit widget id
	 */
	void Clicked(unsigned int widget_id);


  /**
   * @brief Call functions when mouse is released after clicked
	 */
	void Released();

	// RSGizmo Draw

	/**
	* @brief Draw Gizmo for rendering. 
	* Note : This function merged with visible draw
	* @see VisibleDraw
	*/
	void VisibleDrawGizmo();

	/**
	* @brief Draw Gizmo for picking
	* Note : This function merged with hittable draw
	* @see HittableDraw
	*/
	void HittableDrawGizmo();

	/**
	* @brief deallocate all widgets
	* deallocate quad vao, vbo
	* deallocate gizmo, direction view
	*/
	void Shutdown();

	/**
	* @brief Get Widget by widget id
	* @param[in] widget_id_ widget id
	* @return RSWidgetBase* widget
	*/
	RS_N_D RSWidgetBase* GetWidget(unsigned int widget_id_) const;

	/**
	* @brief Get Widget by widget name
	* @param[in] widget_name_ widget name
	* @return RSWidgetBase* widget
	*/
	RS_N_D RSWidgetBase* GetWidget(const std::string& widget_name_) const;

	/**
	 * @brief Get Widget ID by widget name
	 * @param widget_name_ widget name
	 * @return unsigned int widget id
	 */
	RS_N_D unsigned int GetWidgetId(const std::string& widget_name_) const;

	/**
	* @brief Add widget to widget manager
	* widget id is automatically assigned increased by 1 from last widget id
	* @param[in] widget RSWidgetBase pointer
	* @return void
	*/
	void AddWidget(RSWidgetBase* widget);

	/**
	* @brief Add widget to widget manager
	* @param[in] widget_ RSWidgetBase pointer
	* @param[in] widget_id_ widget id
	* @return void
	*/
	void AddWidgetID(RSWidgetBase* widget_, unsigned int widget_id_);

	/**
	* @brief Add Button Widget to widget manager
	* Widget id is automatically assigned increased by 1 from last widget id
	* The widget also is contained in button widgets list
	* @param[in] position_ widget position
	* @param[in] size_ widget size
	* @param[in] name_ widget name
	* @param[in] texture_id_ widget texture id
	* @return Created the widget pointer
	*/
	RSButtonWidget* AddButtonWidget(glm::vec2 position_, glm::vec2 size_, const std::string& name_,
                                  GLuint    texture_id_ = 0);

	/**
	* @brief Add Button Widget to widget manager
	* The widget also is contained in button widgets list
	* @param[in] position_ widget position
	* @param[in] size_ widget size
	* @param[in] name_ widget name
	* @param[in] widget_id_ widget id
	* @param[in] texture_id_ widget texture id
	* @return Created the widget pointer
	*/
	RSButtonWidget* AddButtonWidgetID(glm::vec2    position_, glm::vec2 size_, const std::string& name_,
                                    unsigned int widget_id_, GLuint   texture_id_ = 0);

	/**
	* @brief Add Button Widget to widget manager
	* widget id is automatically assigned increased by 1 from last widget id
	* The widget also is contained in button widgets list
	* @param[in] btn_widget_ RSButtonWidget pointer
	* @return void
	*/
	void AddButtonWidget(RSButtonWidget* btn_widget_);

	/**
	 * @brief Add Button Widget to widget manager
	 * The widget also is contained in button widgets list
	 * @param[in] btn_widget_ RSButtonWidget pointer
	 * @param[in] widget_id_ widget id
	 * @return Created the widget pointer
	 */
	RSButtonWidget* AddButtonWidgetID(RSButtonWidget* btn_widget_, unsigned int widget_id_);

	/**
	 * @brief Add Text Widget to widget manager
	 * The widget also is contained in text widgets list
	 * @param[in] position_ widget position
	 * @param[in] size_ widget size
	 * @param[in] text_ widget text
	 * @param[in] type_ widget type(static or dynamic)
	 * @return Created the widget pointer
	 * @see RSTextStaticDynamic
	 */
	RSTextWidget* AddTextWidget(glm::vec2           position_, float size_, const std::wstring& text_,
                              RSTextStaticDynamic type_ = RSTextStaticDynamic::STATIC);

	/**
	 * @brief Add Text Widget to widget manager with id
	 * The widget also is contained in text widgets list
	 * @param[in] position_ widget position
	 * @param[in] size_ widget size
	 * @param[in] text_ widget text
	 * @param[in] widget_id_ widget id
	 * @param[in] type_ widget type(static or dynamic)
	 * @return Created the widget pointer
	 * @see RSTextStaticDynamic
	 */
	RSTextWidget* AddTextWidgetID(glm::vec2 position_, float size_, const std::wstring& text_, unsigned int widget_id_,
                                RSTextStaticDynamic type_ = RSTextStaticDynamic::STATIC);


	/**
	 * @brief Add Text Widget to widget manager
	 * @param[in] position_ widget position
	 * @param[in] size_ widget size
	 * @param[in] name_ widget name
	 * @param[in] texture_id_ widget texture id
	 * @return Created the widget pointer
	 */
	RSWidgetBase* AddWidget(glm::vec2 position_, glm::vec2 size_, const std::string& name_, GLuint texture_id_ = 0);

	// Add Text Button Widgets
  /**
   * @brief Add Text Button Widget to widget manager
   * The widget also is contained in text button widgets list
   * @param[in] position_ widget position
   * @param[in] size_ widget size
   * @param[in] text_ widget text
   * @param[in] text_size widget text size
   * @param[in] name_ widget name
   * @return Created the widget pointer
   */
  RSTextButtonWidget* AddTextButtonWidget(glm::vec2 position_, glm::vec2 size_, const std::wstring& text_,
                                        const float text_size, const std::string& name_);



  /**
   * @brief Add Text Button Widget to widget manager
   * @param text_btn_widget_ RSTextButtonWidget pointer
	 */
	void AddTextButtonWidget(RSTextButtonWidget* text_btn_widget_);


	/**
	 * @brief Remove widget from widget manager
	 * @param[in] widget RSWidgetBase pointer
	 */
	void RemoveWidget(RSWidgetBase* widget);

	/**
	 * @brief Remove widget from widget manager
	 * @param[in] widget_id_ widget id
	 */
	void RemoveWidget(unsigned int  widget_id_);

	/**
	 * @brief Get gizmo
	 * @return RSGizmo pointer
	 */
	RS_N_D RSGizmo* GetGizmo() const { return m_gizmo; }

	/**
	 * @brief Set gizmo
	 * @param gizmo RSGizmo pointer
	 */
	void SetGizmo(RSGizmo* gizmo) { m_gizmo = gizmo; }

	/**
	 * @brief Get Direction View
	 * @return RSDirectionView pointer
	 */
	RS_N_D RSDirectionView* GetDirectionView() const { return m_direction_view; }
	/**
	 * @brief Set Direction View
	 * @param direction_view RSDirectionView pointer
	 */
	void SetDirectionView(RSDirectionView* direction_view) { m_direction_view = direction_view; }

	RSLegendWidget* GetLegendWidget() const;

	/**
	 * @brief Resize all widgets
	 */
	void ResizeWidgets() const;

	/**
	 * @brief Reset last widget id to 100
	 */
	void ResetLastWidgetId() { last_widget_id = 100; }


protected:
	
	/**
	* @brief Remove widget from all widgets list
	* Note : This function is not remove from button widgets list and text widgets list
	* Call "RemoveWidget" function to remove the widget.
	* This function called from "RemoveWidget" function
	* @param[in] widget_id_ widget id
	* @see RemoveWidget
	*/
	void RemoveFromAllWidget(unsigned int  widget_id_);
	/**
	* @brief Remove widget from button widgets list
	* Note : This function is not remove from all widgets list and text widgets list
	* Call "RemoveWidget" function to remove the widget.
	* This function called from "RemoveWidget" function
	* @param[in] widget_id_ widget id
	* @see RemoveWidget
	*/
	void RemoveFromBtnWidget(unsigned int  widget_id_);
	/**
	* @brief Remove widget from text widgets list
	* Note : This function is not remove from all widgets list and button widgets list
	* Call "RemoveWidget" function to remove the widget.
	* This function called from "RemoveWidget" function
	* @param[in] widget_id_ widget id
	* @see RemoveWidget
	*/
	void RemoveFromTextWidget(unsigned int  widget_id_);


  /**
   * @brief Remove widget from text button widgets list
   * @param widget_id_ widget id
   * @see RemoveWidget
   */
  void RemoveFromTextButtonWidget(unsigned int  widget_id_);

private:
  std::list<RSWidgetBase*> m_all_widgets; ///< All widgets
  std::list<RSButtonWidget*> m_button_widgets; ///< Button widgets
  std::list<RSTextWidget*> m_text_widgets; ///< Text widgets
  std::list<RSTextButtonWidget*> m_text_btn_widgets; ///< Text Button widgets

	// Gizmo pointer
	RSGizmo* m_gizmo = nullptr;

	// Direction View pointer
	RSDirectionView* m_direction_view = nullptr;

	// widget id not used to 0. 0 is null id
	unsigned int last_widget_id; // when add widget, assign this id to widget and increase this value.

  unsigned int quad_vao = 0, quad_vbo = 0; ///< Quad VAO, VBO

	//bool m_any_button_clicked = false;
  unsigned int m_clicked_button_id = 0; ///< Clicked button id

private:
	void InitializeQuad();

	
}; // class WidgetManager


/*
Widget ID list

10 : gizmo translate x
11 : gizmo translate y
12 : gizmo translate z
13 : gizmo translate center
14 : gizmo translate yz
15 : gizmo translate xz
16 : gizmo translate xy

20 : gizmo rotate x
21 : gizmo rotate y
22 : gizmo rotate z

30 : gizmo scale x
31 : gizmo scale y
32 : gizmo scale z
33 : gizmo scale center



40 : camera view iso				// LYJ
41 : camera view top				// HCM
42 : camera view down			// KJH
43 : camera view front				// EJW
44 : camera view back				// PSH
45 : camera view right				// KCY // ddungi
46 : camera view left				// PJS // jordy
47 : camera view perspective	// PJY

50 : direction view x
51 : direction view y
52 : direction view z
53 : direction view center

60 : legend view

*/
}

#endif // !RS_WIDGET_MANAGER_H_