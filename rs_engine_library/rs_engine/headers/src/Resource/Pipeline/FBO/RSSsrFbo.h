/*********************************************************************
 * \file   RSSsrFbo.h
 * \brief  RS Screen Space Reflection class for rendering system.
 * 
 * \author jinseob Park
 * \date   2025/03/19
 *********************************************************************/
#ifndef RS_SSR_H_
#define RS_SSR_H_

#include "RSFBO.h"

 /**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
  */
namespace _RS_Internal
{
  /**
   * @brief SSR class
   * This class is for screen space reflection.
   */
  class RSSsrFbo : public RSFBO
  {
  public:
    RSSsrFbo() = default;
    ~RSSsrFbo() override = default;

    /**
     * @brief Create SSR buffers and textures
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void CreateFbo(const int width_, const int height_) override;

    /**
     * @brief Resize SSR texture
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void Resize(const int width_, const int height_) override;

    /**
     * @brief Release SSR buffers and textures
     */
    void ReleaseFbo() override;

    /**
     * @brief Bind SSR frame buffer
     */
    void BindFbo() const override;

    /**
     * @brief Get SSR texture
     * @return SSR texture buffer id (uint)
     */
    unsigned int GetSsrTexture() const { return ssr_texture; }

    /**
     * @brief Get SSR frame buffer
     * @return SSR frame buffer id (uint)
     */
    unsigned int GetSsrFrameBuffer() const { return ssr_frame_buffer; }

  private:
    unsigned int ssr_frame_buffer = 0; ///< SSR frame buffer id
    unsigned int ssr_texture = 0; ///< SSR texture id

    bool m_dirty = true; ///< Dirty flag for SSR kernel
  };

} // namespace _RS_Internal

#endif // RS_SSR_H_
