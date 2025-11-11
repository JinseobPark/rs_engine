/******************************************************************************/
/*!
\file   RSGraphicsData.h
\author Jinseob Park
\date   2024/08/01

This file contains data structure for graphics data.

*/
/******************************************************************************/
#ifndef RS_GRAPHICS_DATA_H_
#define RS_GRAPHICS_DATA_H_

#include "glm/glm.hpp"

#define SKYBOX_TIMER_VALUE 0.0f

/**
 * @brief Render Type enum list.
 */
enum class RSDataRenderType : int
{
  //NONE = 0x00000000,
  //FORWARD = 1 << 0,
  //DEFERRED = 1 << 1,
  //STREAM_LINE = 1 << 2,
  //PATH_LINE = 1 << 3,
  //VECTOR_FIELD = 1 << 4,
  //CONTOUR = 1 << 5,
  //ISOSURFACE = 1 << 6

  NONE = 0,
  FORWARD,
  DEFERRED,
  STREAM_LINE,
  PATH_LINE,
  VECTOR_FIELD,
  CONTOUR,
  ISOSURFACE
};


/**
 * @brief Graphics Data Structure
 * Include background color,
 * light direction, light position, light space matrix, light projection, light view, 
 * pixel id, is show skybox, skybox timer value, is use stencil buffer.
 */
struct RSGraphicsData
{
  float clear_color[3] = { 0.8f, 1.0f, 0.8f }; // @brief clear color
	//glm::vec3 light_dir = glm::vec3(0.0f, -1.0f, 0.5f);
  glm::vec3 light_pos = glm::vec3(10.0f, 30.0f, 10.0f); // @brief light position
  glm::mat4 light_space_matrix = glm::mat4(1.0f); // @brief light space matrix
  glm::mat4 light_projection = glm::mat4(1.0f); // @brief light projection
  glm::mat4 light_view = glm::mat4(1.0f); // @brief light view

  GLuint m_pixel_id = 0; // @brief pixel id
  bool is_show_skybox = true; // @brief is show skybox
  float skybox_timer_value = SKYBOX_TIMER_VALUE; // @brief skybox timer value

	// use this later.
  bool is_use_stencil_buffer = false; // @brief is use stencil buffer

  // Dirty flag for light update.
  bool b_light_dirty = true;

  float g_max_thickness = 0.6f; // @brief g_max_thickness
  float g_depthbias = 0.06f; // @brief g_depthbias
  float g_blur_coefficient = 6.0f; // @brief g_blur_coefficient

  RSDataRenderType data_render_type = RSDataRenderType::FORWARD; // @brief render type

  /**
   * @brief Reset Graphics Data
	 */
	void Reset()
	{
		clear_color[0] = 0.8f;
		clear_color[1] = 1.0f;
		clear_color[2] = 0.8f;

		light_pos = glm::vec3(10.0f, 30.0f, 10.0f);
    light_space_matrix = glm::mat4(1.0f);
    light_projection = glm::mat4(1.0f);
    light_view = glm::mat4(1.0f);

    m_pixel_id = 0;
    //is_show_skybox = true;
    skybox_timer_value = SKYBOX_TIMER_VALUE;
    b_light_dirty = true;
	}

  void SetLightUpdated(const bool is_dirty)
  {
    b_light_dirty = is_dirty;
  }

}; // struct RSGraphicsData


/**
 * @brief Rendering flag enumeration.
 * It allows to overlap flags by bitwise operation.
 * list : NONE, SHADOW, DEFERRED, SSAO
 * later : BLOOM, SSR, VOLUMETRIC_LIGHTING, SSF
 *
 * IF deferred flag is off, then forward rendering.
 */
enum class RenderingFlag : uint32_t
{
  NONE = 0,
  SHADOW = 1 << 0,
  OPAQUE_DEFERRED = 1 << 1,
  FLUID_DEFERRED = 1 << 2,
  SSAO = 1 << 3,
  SKYBOX = 1 << 4,
  SSR = 1 << 5,
};

static bool operator&(RenderingFlag lhs, RenderingFlag rhs)
{
  return static_cast<bool>(static_cast<std::underlying_type_t<RenderingFlag>>(lhs) &
                           static_cast<std::underlying_type_t<RenderingFlag>>(rhs));
}

inline RenderingFlag operator|(RenderingFlag lhs, RenderingFlag rhs)
{
  return static_cast<RenderingFlag>(static_cast<std::underlying_type_t<RenderingFlag>>(lhs) |
                                    static_cast<std::underlying_type_t<RenderingFlag>>(rhs));
}

inline RenderingFlag operator|=(RenderingFlag& lhs, RenderingFlag rhs)
{
  lhs = static_cast<RenderingFlag>(static_cast<std::underlying_type_t<RenderingFlag>>(lhs) |
                                   static_cast<std::underlying_type_t<RenderingFlag>>(rhs));
  return lhs;
}

inline RenderingFlag operator&=(RenderingFlag& lhs, RenderingFlag rhs)
{
  lhs = static_cast<RenderingFlag>(static_cast<std::underlying_type_t<RenderingFlag>>(lhs) &
                                   static_cast<std::underlying_type_t<RenderingFlag>>(rhs));
  return lhs;
}

inline RenderingFlag operator~(RenderingFlag rhs)
{
  return static_cast<RenderingFlag>(~static_cast<std::underlying_type_t<RenderingFlag>>(rhs));
}



#endif // RS_GRAPHICS_DATA_H_