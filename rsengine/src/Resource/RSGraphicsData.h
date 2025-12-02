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
 * @brief Kernel preset types for image post-processing
 */
enum class KernelPreset : uint8_t
{
  NONE = 0,       ///< Identity (no effect)
  EDGE_DETECT,    ///< Edge detection kernel
  SHARPEN,        ///< Sharpening kernel
  BOX_BLUR,       ///< 3x3 Box blur
  GAUSSIAN_BLUR,  ///< 3x3 Gaussian blur approximation
  EMBOSS,         ///< Emboss effect
  CUSTOM          ///< User-defined custom kernel
};

/**
 * @brief Kernel post-process data structure
 * Contains 3x3 kernel matrix, divisor, offset, and pass count
 */
struct RSKernelPostProcessData
{
  float kernel[9] = {    ///< 3x3 kernel matrix (row-major)
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f
  };
  float divisor = 1.0f;  ///< Kernel divisor (normalization)
  float offset = 0.0f;   ///< Color offset after convolution
  int pass_count = 1;    ///< Number of passes (1-4, for chaining)
  KernelPreset preset = KernelPreset::NONE;  ///< Current preset
  bool is_dirty = true;  ///< Flag to update UBO

  /**
   * @brief Set kernel from preset
   * @param preset_ Kernel preset type
   */
  void SetPreset(KernelPreset preset_)
  {
    preset = preset_;
    is_dirty = true;
    switch (preset_)
    {
    case KernelPreset::NONE:
      // Identity kernel
      kernel[0] = 0.0f; kernel[1] = 0.0f; kernel[2] = 0.0f;
      kernel[3] = 0.0f; kernel[4] = 1.0f; kernel[5] = 0.0f;
      kernel[6] = 0.0f; kernel[7] = 0.0f; kernel[8] = 0.0f;
      divisor = 1.0f; offset = 0.0f;
      break;
    case KernelPreset::EDGE_DETECT:
      // Laplacian edge detection
      kernel[0] = -1.0f; kernel[1] = -1.0f; kernel[2] = -1.0f;
      kernel[3] = -1.0f; kernel[4] =  8.0f; kernel[5] = -1.0f;
      kernel[6] = -1.0f; kernel[7] = -1.0f; kernel[8] = -1.0f;
      divisor = 1.0f; offset = 0.0f;
      break;
    case KernelPreset::SHARPEN:
      kernel[0] =  0.0f; kernel[1] = -1.0f; kernel[2] =  0.0f;
      kernel[3] = -1.0f; kernel[4] =  5.0f; kernel[5] = -1.0f;
      kernel[6] =  0.0f; kernel[7] = -1.0f; kernel[8] =  0.0f;
      divisor = 1.0f; offset = 0.0f;
      break;
    case KernelPreset::BOX_BLUR:
      kernel[0] = 1.0f; kernel[1] = 1.0f; kernel[2] = 1.0f;
      kernel[3] = 1.0f; kernel[4] = 1.0f; kernel[5] = 1.0f;
      kernel[6] = 1.0f; kernel[7] = 1.0f; kernel[8] = 1.0f;
      divisor = 9.0f; offset = 0.0f;
      break;
    case KernelPreset::GAUSSIAN_BLUR:
      // 3x3 Gaussian approximation
      kernel[0] = 1.0f; kernel[1] = 2.0f; kernel[2] = 1.0f;
      kernel[3] = 2.0f; kernel[4] = 4.0f; kernel[5] = 2.0f;
      kernel[6] = 1.0f; kernel[7] = 2.0f; kernel[8] = 1.0f;
      divisor = 16.0f; offset = 0.0f;
      break;
    case KernelPreset::EMBOSS:
      kernel[0] = -2.0f; kernel[1] = -1.0f; kernel[2] = 0.0f;
      kernel[3] = -1.0f; kernel[4] =  1.0f; kernel[5] = 1.0f;
      kernel[6] =  0.0f; kernel[7] =  1.0f; kernel[8] = 2.0f;
      divisor = 1.0f; offset = 0.5f;
      break;
    case KernelPreset::CUSTOM:
      // Keep current kernel values
      break;
    }
  }

  /**
   * @brief Reset to identity kernel
   */
  void Reset()
  {
    SetPreset(KernelPreset::NONE);
    pass_count = 1;
  }
};

/**
 * @brief Color filter mode for image post-processing
 */
enum class ColorFilterMode : uint8_t
{
  NONE = 0,         ///< No filter (original colors)
  GRAYSCALE,        ///< Grayscale (luminance)
  RED_CHANNEL,      ///< Red channel only
  GREEN_CHANNEL,    ///< Green channel only
  BLUE_CHANNEL,     ///< Blue channel only
  SEPIA,            ///< Sepia tone
  INVERT,           ///< Invert colors
  CUSTOM            ///< Custom color weights
};

/**
 * @brief Color filter data structure
 * Contains filter mode and custom weights for color manipulation
 */
struct RSColorFilterData
{
  ColorFilterMode mode = ColorFilterMode::NONE;  ///< Current filter mode
  float weights[3] = { 0.299f, 0.587f, 0.114f }; ///< RGB weights for grayscale. From BT.601 (NTSC standard)
  float intensity = 1.0f;                         ///< Filter intensity (0=original, 1=full)
  bool is_dirty = true;                           ///< Flag to update UBO

  /**
   * @brief Set filter mode and apply preset weights
   */
  void SetMode(ColorFilterMode mode_)
  {
    mode = mode_;
    is_dirty = true;
    switch (mode_)
    {
    case ColorFilterMode::NONE:
      weights[0] = 1.0f; weights[1] = 1.0f; weights[2] = 1.0f;
      break;
    case ColorFilterMode::GRAYSCALE:
      weights[0] = 0.299f; weights[1] = 0.587f; weights[2] = 0.114f;
      break;
    case ColorFilterMode::RED_CHANNEL:
      weights[0] = 1.0f; weights[1] = 0.0f; weights[2] = 0.0f;
      break;
    case ColorFilterMode::GREEN_CHANNEL:
      weights[0] = 0.0f; weights[1] = 1.0f; weights[2] = 0.0f;
      break;
    case ColorFilterMode::BLUE_CHANNEL:
      weights[0] = 0.0f; weights[1] = 0.0f; weights[2] = 1.0f;
      break;
    case ColorFilterMode::SEPIA:
      weights[0] = 1.2f; weights[1] = 1.0f; weights[2] = 0.8f;
      break;
    case ColorFilterMode::INVERT:
      weights[0] = -1.0f; weights[1] = -1.0f; weights[2] = -1.0f;
      break;
    case ColorFilterMode::CUSTOM:
      break;
    }
  }

  void Reset()
  {
    SetMode(ColorFilterMode::NONE);
    intensity = 1.0f;
  }
};

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
  IMAGE_KERNEL = 1 << 6,   ///< Kernel-based image post-processing
  COLOR_FILTER = 1 << 7,   ///< Color filter post-processing (grayscale, channels, etc.)
  CLOTH_DEFERRED = 1 << 8, ///< Cloth simulation deferred rendering
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