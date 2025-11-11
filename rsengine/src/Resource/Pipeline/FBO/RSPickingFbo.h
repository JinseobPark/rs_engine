/*********************************************************************
 * \file   RSPickingFbo.h
 * \brief  RS engine Picking Frame buffer object class for rendering system.
 * 
 * \author Jinseob Park
 * \date   2025/03/13
 *********************************************************************/

#ifndef RS_PICKING_FBO_H_
#define RS_PICKING_FBO_H_

#include "RSFBO.h"

 /**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
  */
namespace _RS_Internal
{

  /**
   * @brief Picking Frame buffer object class
   */
  class RSPickingFbo : public RSFBO
  {
  public:
    RSPickingFbo()           = default;
    ~RSPickingFbo() override = default;

    /**
     * @brief Create Picking buffers and textures
     */
    void CreateFbo(const int width_, const int height_) override;

    /**
     * @brief Resize Picking texture
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void Resize(int width_, int height_) override;

    /**
     * @brief Release Picking buffers and textures
     */
    void ReleaseFbo() override;

    /**
     * @brief Bind Picking frame buffer
     */
    void BindFbo() const override;

    /**
     * @brief　Get Picking frame buffer
     * @return　Picking frame buffer id (uint)
     */
    RS_N_D unsigned int GetPickingFrameBuffer() const { return picking_frame_buffer; }

    /**
     * @brief Get Picking texture
     * @return Picking texture buffer id (uint)
     */
    RS_N_D unsigned int GetPickingTexture() const { return picking_texture; }

  private:
    unsigned int picking_frame_buffer = 0; ///< Picking frame buffer id
    unsigned int picking_texture = 0;     ///< Picking texture buffer id
    unsigned int picking_rbo = 0;        ///< Picking render buffer object id
  };
  
}

#endif  // !RS_PICKING_FBO_H_
