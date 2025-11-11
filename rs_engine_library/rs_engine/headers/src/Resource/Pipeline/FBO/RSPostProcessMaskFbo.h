/*********************************************************************
 * \file   RSPostProcessMaskFbo.h
 * \brief  Post Process Mask FBO Class for rendering system
 * This class is for post process mask frame buffer object.
 * The masking targets are opaque, fluid, and transparent.
 * 
 * \author Jinseob Park
 * \date   2025/04/15
 *********************************************************************/
#ifndef RS_POST_PROCESS_MASK_FBO_H_
#define RS_POST_PROCESS_MASK_FBO_H_

#include "RSFBO.h"
/**
 * @brief Rendering System Internal namespace
 */
namespace _RS_Internal
{
  /**
   * @brief Post Process Mask FBO class
   */
  class RSPostProcessMaskFbo : public RSFBO
  {
  public:
    RSPostProcessMaskFbo() = default;
    ~RSPostProcessMaskFbo() override = default;
    /**
     * @brief Create Post Process Mask FBO
     */
    void CreateFbo(const int width_, const int height_) override;
    /**
     * @brief Resize Post Process Mask FBO
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void Resize(int width_, int height_) override;
    /**
     * @brief Release Post Process Mask FBO
     */
    void ReleaseFbo() override;
    /**
     * @brief Bind Post Process Mask FBO
     */
    void BindFbo() const override;

    /**
     * @brief Get Post Process Mask FBO id
     * @return Post Process Mask FBO id (uint)
     */
    unsigned int GetPostProcessMaskTexture() const { return m_post_process_mask_texture; }

    /**
     * @brief Get Post Process Mask FBO id
     * @return Post Process Mask FBO id (uint)
     */
    bool IsDirty() const { return m_dirty; }

    /**
     * @brief Set dirty flag for Post Process Mask FBO
     * @param dirty_ Dirty flag
     */
    void SetDirty(const bool dirty_) { m_dirty = dirty_; }

  private:
    unsigned int m_post_process_mask_fbo = 0; // Post Process Mask FBO id
    unsigned int m_post_process_mask_texture = 0; // Post Process Mask texture id

    bool m_dirty = true; // Dirty flag for Post Process Mask FBO
  };


} // namespace _RS_Internal

#endif