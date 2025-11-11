/*********************************************************************************************************************/
/*!
\file   RSWidgetData.h
\author Jinseob Park
\date   2025/02/11

This file contains all definition data for widget.

*/
/*********************************************************************************************************************/

#ifndef RS_WIDGET_DATA_H_
#define RS_WIDGET_DATA_H_

/**
 * @brief Gizmo Axis Type enum class
 */
enum class GizmoAxisType : std::uint8_t
{
  CENTER = 0,
  X_AXIS,
  Y_AXIS,
  Z_AXIS,
  X_AXIS_INV,
  Y_AXIS_INV,
  Z_AXIS_INV,
};

/**
 * @brief Text static dynamic enum class
 * Static - text is not changed dynamically
 * Dynamic - text is changed dynamically
 * When text isn't changed dynamically, please use static
 */
enum class RSTextStaticDynamic : std::uint8_t
{
	STATIC = 0,
	DYNAMIC
};

/**
 * @brief Gizmo state enum class
 */
enum class RSGizmoState : std::uint8_t
{
  NONE = 0,
  TRANSLATION,
  ROTATION,
  SCALE
};

/**
 * @brief Widget Visibility
 * It includes hit visible, none hit visible, hit invisible, none hit invisible
 */
enum class RSVisibility : std::uint8_t
{
  HIT_VISIBLE = 0,
  NONE_HIT_VISIBLE,
  HIT_INVISIBLE,
  NONE_HIT_INVISIBLE
};

/**
 * @brief Widget Text Alignment Type
 */
enum class RSTextAlignment : std::uint8_t
{
  MIDDLE = 0,
  LEFT,
  RIGHT
};

#endif
