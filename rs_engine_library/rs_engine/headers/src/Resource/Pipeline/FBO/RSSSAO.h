/*********************************************************************
 * \file   RSSSAO.h
 * \brief  SSAO class for rendering system.
 * Screen space ambient occlusion class for rendering system.
 * \author jinseob Park
 * \date   2025/03/12
 *********************************************************************/
#ifndef RSSSAO_H_
#define RSSSAO_H_

#include "RSFBO.h"
 /**
  * @brief SSAO class
  * This class is for screen space ambient occlusion.
  */
class RSSSAO : public RSFBO
{
public:
  RSSSAO() = default;
  ~RSSSAO() = default;


  /**
   * @brief Create SSAO buffers and textures include noise texture
   */
  void CreateFbo() override;

  /**
   * @brief Resize SSAO texture and Blur texture
   * @param width_ Application window width
   * @param height_ Application window height
   */
  void ResizeBuffers(int width_, int height_);

  /**
   * @brief Release SSAO buffers and textures include noise texture
   */
  void ReleaseFbo() override;

  /**
   * @brief Bind SSAO frame buffer
   */
  void BindFbo() const;

  /**
   * @brief Bind Blur frame buffer
   */
  void BindBlurFbo() const;

  /**
   * @brief Get SSAO texture
   * @return SSAO texture buffer id (uint)
   */
  unsigned int GetTexture() const { return ssao_texture; }

  /**
   * @brief Get SSAO blur texture
   * @return SSAO blur texture buffer id (uint)
   */
  unsigned int GetBlurTexture() const { return ssao_blur_texture; }

  /**
   * @brief Get Noise texture
   * @return Noise texture buffer id (uint)
   */
  unsigned int GetNoiseTexture() const { return noise_texture; }


private:
  unsigned int ssao_frame_buffer = 0; ///< SSAO frame buffer id
  unsigned int ssao_blur_frame_buffer = 0; ///< SSAO blur frame buffer id

  unsigned int ssao_texture = 0; ///< SSAO texture id
  unsigned int ssao_blur_texture = 0; ///< SSAO blur texture id

  unsigned int noise_texture = 0; ///< Noise texture buffer id
};


#endif
