/*********************************************************************
 * \file   RSShadowMapFbo.h
 * \brief  RS engine Shadow Map Frame buffer object class for rendering system.
 * 
 * \author Jinseob Park
 * \date   2025/03/13
 *********************************************************************/

#ifndef RS_SHADOW_MAP_FBO_H_
#define RS_SHADOW_MAP_FBO_H_

#include "RSFBO.h"

 /**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
  */
namespace _RS_Internal
{
  /**
   * @brief Shadow Map Frame buffer object
   */
  class RSShadowMapFbo : public RSFBO
  {
  public:
    RSShadowMapFbo()           = default;
    ~RSShadowMapFbo() override = default;
    /**
     * @brief Create Shadow Map Frame buffer object
     */
    void CreateFbo(const int width_, const int height_) override;
    /**
     * @brief Resize Shadow Map texture
     * @param width_ texture width
     * @param height_ texture height
     */
    void Resize(int width_, int height_) override;
    /**
     * @brief Release Shadow Map buffers
     */
    void ReleaseFbo() override;
    /**
     * @brief Bind Shadow Map frame buffer
     */
    void BindFbo() const override;

    /**
     * @brief Change Shadow Map texture to default texture
     */
    void ClearShadowMap();

    /**
     * @brief Get Shadow Map texture
     * @return Shadow Map texture buffer id (uint)
     */
    RS_N_D unsigned int GetShadowMapTexture() const { return shadow_map_texture; }
    /**
     * @brief Get Shadow Map Frame buffer
     * @return Shadow Map frame buffer id (uint)
     */
    RS_N_D unsigned int GetShadowMapFrameBuffer() const { return shadow_map_frame_buffer; }


    /**
     * @brief Get Shadow Map width
     * @return Shadow Map width (uint)
     */
    RS_N_D int GetShadowWidth() const { return shadow_width; }

    /**
     * @brief Get Shadow Map height
     * @return Shadow Map height (uint)
     */
    RS_N_D int GetShadowHeight() const { return shadow_height; }



  private:
    unsigned int shadow_width = 2048, shadow_height = 2048;	///< Shadow Buffer mapping width, height
    unsigned int shadow_map_frame_buffer = 0;	///< Shadow Map frame buffer id
    unsigned int shadow_map_texture = 0;		///< Shadow Map texture buffer id
  };

  
} // namespace _RS_Internal


#endif // !RS_SHADOW_MAP_FBO_H_
