/*********************************************************************
 * \file   RSDeferredFbo.h
 * \brief  RS engine Deferred Frame buffer object class for rendering system.
 * 
 * \author jinseob Park
 * \date   2025/03/13
 *********************************************************************/
#ifndef RS_DEFERRED_FBO_H_
#define RS_DEFERRED_FBO_H_

#include "RSFBO.h"
 /**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
  */
namespace _RS_Internal
{
  /**
   * @brief Deferred Frame buffer object
   */
  class RSDeferredFbo : public RSFBO
  {
  public:
    RSDeferredFbo() = default;
    ~RSDeferredFbo() override = default;
    /**
     * @brief Create Deferred Frame buffer object
     */
    void CreateFbo(const int width_, const int height_) override;
    /**
     * @brief Resize Deferred texture
     * @param width_ texture width
     * @param height_ texture height
     */
    void Resize(int width_, int height_) override;
    /**
     * @brief Release Deferred buffers
     */
    void ReleaseFbo() override;
    /**
     * @brief Bind Deferred frame buffer
     */
    void BindFbo() const override;

    /**
     * @brief Get Deferred Frame buffer
     * @return Deferred frame buffer id (uint)
     */
    RS_N_D unsigned int GetDeferredFrameBuffer() const { return deferred_frame_buffer; }
    /**
     * @brief Get Deferred Position texture
     * @return Deferred Position texture buffer id (uint)
     */
    RS_N_D unsigned int GetDeferredPositionTexture() const { return deferred_position_texture; }
    /**
     * @brief Get Deferred Normal texture
     * @return Deferred Normal texture buffer id (uint)
     */
    RS_N_D unsigned int GetDeferredNormalTexture() const { return deferred_normal_texture; }
    /**
     * @brief Get Deferred Albedo texture
     * @return Deferred Albedo texture buffer id (uint)
     */
    RS_N_D unsigned int GetDeferredAlbedoTexture() const { return deferred_albedo_texture; }
    /**
     * @brief Get Deferred Specular texture
     * @return Deferred Specular texture buffer id (uint)
     */
    //RS_N_D unsigned int GetCustomDepthTexture() const { return deferred_depth_texture; }


    /** ************************************************************************************** **/
    /**
     * @brief Get Fluid Deferred Position texture
     * @return Fluid Deferred Position texture buffer id (uint)
     */
    RS_N_D unsigned int GetFluidDeferredPositionTexture() const { return fluid_deferred_position_texture; }

    /**
     * @brief Get Fluid Deferred Albedo texture
     * @return Fluid Deferred Albedo texture buffer id (uint)
     */
    RS_N_D unsigned int GetFluidDeferredNormalTexture() const { return fluid_deferred_normal_texture; }

    /**
     * @brief Get Fluid Deferred Albedo texture
     * @return Fluid Deferred Albedo texture buffer id (uint)
     */
    RS_N_D unsigned int GetFluidDeferredAlbedoTexture() const { return fluid_deferred_albedo_texture; }

    /** ************************************************************************************** **/

    /**
     * @brief Get Fluid Deferred Depth texture
     * @return Fluid Deferred Depth texture buffer id (uint)
     */
    RS_N_D unsigned int GetCustomDeferredDepthTexture() const { return custom_deferred_depth_texture; }


    /**
     * @brief Get Depth Texture
     * @return Depth texture buffer id (uint)
     */
    RS_N_D unsigned int GetDepthTexture() const { return depth_texture; } ///< Depth texture buffer id

    /**
     * @brief Get Custom Mask Texture
     * @return Custom Mask texture buffer id (uint)
     */
    RS_N_D unsigned int GetCustomMaskTexture() const { return m_custom_mask_texture; }

  private:
    unsigned int deferred_frame_buffer = 0; ///< Deferred frame buffer id

    unsigned int deferred_position_texture = 0; ///< Deferred position texture buffer id
    unsigned int deferred_normal_texture = 0; ///< Deferred normal texture buffer id
    unsigned int deferred_albedo_texture = 0; ///< Deferred albedo texture buffer id
    //unsigned int deferred_depth_texture = 0; ///< Deferred depth texture buffer id (custom)

    unsigned int fluid_deferred_position_texture = 0; ///< Fluid position texture id
    unsigned int fluid_deferred_normal_texture = 0; ///< Fluid normal texture id
    unsigned int fluid_deferred_albedo_texture = 0; ///< Fluid albedo texture id
    //unsigned int fluid_deferred_depth_texture = 0; ///< Fluid depth texture id

    // custom depth texture id. It includes opaque and fluid depth to R,G channel
    unsigned int custom_deferred_depth_texture = 0; 

    unsigned int m_custom_mask_texture = 0; ///< custom mask texture buffer id. post process mask

    unsigned int depth_texture = 0; ///< depth texture buffer id (general)
  };
  
}


#endif // !RS_DEFERRED_FBO_H_
