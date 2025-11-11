/*********************************************************************
 * \file   RSSsaoBlurFbo.h
 * \brief  SSAO Blur FBO class for rendering system.
 * Screen space ambient occlusion blur FBO class for rendering system.
 * \author Jinseob Park
 * \date   2025/03/18
 *********************************************************************/
#ifndef RS_SSAO_BLUR_FBO_H_
#define RS_SSAO_BLUR_FBO_H_

#include "RSFBO.h"

namespace _RS_Internal
{
  /**
   * @brief SSAO Blur FBO class
   * This class is for screen space ambient occlusion blur FBO.
   */
  class RSSsaoBlurFbo : public RSFBO
  {
  public:
    RSSsaoBlurFbo()           = default;
    ~RSSsaoBlurFbo() override = default;
    /**
     * @brief Create SSAO blur buffers and textures
     */
    void CreateFbo(const int width_, const int height_) override;

    /**
     * @brief Resize SSAO blur texture
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void Resize(int width_, int height_) override;

    /**
     * @brief Release SSAO blur buffers and textures
     */
    void ReleaseFbo() override;

    /**
     * @brief Bind SSAO blur frame buffer
     */
    void BindFbo() const override;
   
    /**
     * @brief Get SSAO blur texture
     * @return SSAO blur texture id
     */
    unsigned int GetSsaoBlurTexture() const { return ssao_blur_texture; }
  private:
    unsigned int ssao_blur_frame_buffer = 0; ///< SSAO blur frame buffer id
    unsigned int ssao_blur_texture = 0;      ///< SSAO blur texture id
  };
  
} // namespace _RS_Internal

#endif
