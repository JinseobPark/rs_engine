/*********************************************************************
 * \file   RSFinalFbo.h
 * \brief  RS engine Final Frame buffer object class for rendering system.
 * 
 * \author Jinseob Park
 * \date   2025/03/13
 *********************************************************************/
#ifndef RS_FINAL_FBO_H_
#define RS_FINAL_FBO_H_

#include "RSFBO.h"

 /**
   * @brief Rendering System Internal namespace
   * This namespace is for internal use only.
   */
namespace _RS_Internal
{
  /**
   * @brief Final Frame buffer object class
   */
  class RSFinalFbo : public RSFBO
  {
  public:
    RSFinalFbo()           = default;
    ~RSFinalFbo() override = default;
    /**
     * @brief Create Final buffers and textures
     */
    void CreateFbo(const int width_, const int height_) override;
    /**
     * @brief Resize Final texture
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void Resize(int width_, int height_) override;
    /**
     * @brief Release Final buffers and textures
     */
    void ReleaseFbo() override;
    /**
     * @brief Bind Final frame buffer
     */
    void BindFbo() const override;
    /**
     * @brief Get Final texture
     * @return Final texture buffer id (uint)
     */
    RS_N_D unsigned int GetFinalTexture() const { return final_texture_buffer; }

    /**
     * @brief　Get Final frame buffer
     * @return　Final frame buffer id (uint)
     */
    unsigned int GetFinalFrameBuffer() const { return final_frame_buffer; }

  private:
    unsigned int final_frame_buffer = 0; ///< Final frame buffer id
    unsigned int final_texture_buffer = 0; ///< Final texture buffer id
    unsigned int final_render_buffer = 0; ///< Final render buffer object id
  };
  
}

#endif // !RS_FINAL_FBO_H_
