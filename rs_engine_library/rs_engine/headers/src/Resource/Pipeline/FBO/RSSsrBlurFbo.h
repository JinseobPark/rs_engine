/*****************************************************************//**
 * \file   RSSsrBlurFbo.h
 * \brief  SSR Blur Fbo class for rendering system.
 * Screen space reflection blur FBO class for rendering system.
 * 
 * \author Jinseob Park
 * \date   2025/04/03
 *********************************************************************/
#ifndef RS_SSR_BLUR_FBO_H_
#define RS_SSR_BLUR_FBO_H_

#include "RSFBO.h"

namespace _RS_Internal
{
  /**
   * @brief SSR Blur FBO class
   * This class is for screen space reflection blur FBO.
   */
  class RSSsrBlurFbo : public RSFBO
  {
  public:
    RSSsrBlurFbo()           = default;
    ~RSSsrBlurFbo() override = default;
    /**
     * @brief Create SSR blur buffers and textures
     */
    void CreateFbo(const int width_, const int height_) override;
    /**
     * @brief Resize SSR blur texture
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void Resize(int width_, int height_) override;
    /**
     * @brief Release SSR blur buffers and textures
     */
    void ReleaseFbo() override;
    /**
     * @brief Bind SSR blur frame buffer
     */
    void BindFbo() const override;
    /**
     * @brief Get SSR blur texture
     * @return SSR blur texture id
     */
    unsigned int GetSsrBlurTexture() const { return ssr_blur_texture; }
  private:
    unsigned int ssr_blur_frame_buffer = 0; ///< SSR blur frame buffer id
    unsigned int ssr_blur_texture = 0;      ///< SSR blur texture id
  };
  
}

#endif
