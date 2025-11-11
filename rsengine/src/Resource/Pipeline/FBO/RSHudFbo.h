/*****************************************************************//**
 * \file   RSHudFbo.h
 * \brief  RS engine HUD Frame buffer object class for rendering system.
 * 
 * \author Jinseob Park
 * \date   2025/03/13
 *********************************************************************/
#ifndef RS_HUD_FBO_H_
#define RS_HUD_FBO_H_

#include "RSFBO.h"

 /**
   * @brief Rendering System Internal namespace
   * This namespace is for internal use only.
   */
namespace _RS_Internal
{
  /**
   * @brief HUD Frame buffer object class
   */
  class RSHudFbo : public RSFBO
  {
  public:
    RSHudFbo() = default;
    ~RSHudFbo() override = default;
    /**
     * @brief Create HUD Frame buffer object
     */
    void CreateFbo(const int width_, const int height_) override;
    /**
     * @brief Resize HUD texture
     * @param width_ texture width
     * @param height_ texture height
     */
    void Resize(int width_, int height_) override;
    /**
     * @brief Release HUD buffers
     */
    void ReleaseFbo() override;
    /**
     * @brief Bind HUD frame buffer
     */
    void BindFbo() const override;
    /**
     * @brief Get HUD texture
     * @return HUD texture buffer id (uint)
     */
    RS_N_D unsigned int GetHudTexture() const { return hud_texture; }
    /**
     * @brief Get HUD Frame buffer
     * @return HUD frame buffer id (uint)
     */
    RS_N_D unsigned int GetHudFrameBuffer() const { return hud_frame_buffer; }

  private:
    unsigned int hud_frame_buffer = 0; ///< HUD frame buffer id
    unsigned int hud_texture = 0; ///< HUD texture buffer id
  };
  
}

#endif // !RS_HUD_FBO_H_
