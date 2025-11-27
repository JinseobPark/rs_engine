/*********************************************************************
 * \file   RSPostProcessFbo.h
 * \brief  Post-process FBO class for kernel-based image processing.
 *         Supports ping-pong rendering for multi-pass kernel effects.
 * \author jinseob Park
 * \date   2025/11/27
 *********************************************************************/
#ifndef RS_POST_PROCESS_FBO_H_
#define RS_POST_PROCESS_FBO_H_

#include "RSFBO.h"

/**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
  */
namespace _RS_Internal
{

/**
  * @brief Post-process FBO class
  * This class is for kernel-based image post-processing (ping-pong buffer).
  */
class RSPostProcessFbo : public RSFBO
{
public:
  RSPostProcessFbo()           = default;
  ~RSPostProcessFbo() override = default;

  /**
   * @brief Create post-process framebuffer and color texture
   * @param width_ Application window width
   * @param height_ Application window height
   */
  void CreateFbo(int width_, int height_) override;

  /**
   * @brief Resize post-process texture
   * @param width_ Application window width
   * @param height_ Application window height
   */
  void Resize(int width_, int height_) override;

  /**
   * @brief Release post-process framebuffer and textures
   */
  void ReleaseFbo() override;

  /**
   * @brief Bind post-process framebuffer
   */
  void BindFbo() const override;

  /**
   * @brief Get color texture
   * @return Color texture buffer id (uint)
   */
  unsigned int GetColorTexture() const { return m_color_texture; }

private:
  unsigned int m_frame_buffer = 0;   ///< Post-process framebuffer id
  unsigned int m_color_texture = 0;  ///< Post-process color texture id
};

} // namespace _RS_Internal

#endif // RS_POST_PROCESS_FBO_H_
