/*********************************************************************
 * \file   RSSkyboxFbo.h
 * \brief  Skybox Frame buffer object class for rendering system.
 * 
 * \author Jinseob Park
 * \date   2025/03/13
 *********************************************************************/
#ifndef RS_SKYBOX_FBO_H_
#define RS_SKYBOX_FBO_H_
#include "RSFBO.h"

 /**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
  */
namespace _RS_Internal
{
  /**
   * @brief Skybox Frame buffer object class
   */
  class RSSkyboxFbo : public RSFBO
  {
  public:
    RSSkyboxFbo()           = default;
    ~RSSkyboxFbo() override = default;
    /**
     * @brief Create Skybox buffers and textures
     */
    void CreateFbo(const int width_, const int height_) override;
    /**
     * @brief Resize Skybox texture
     * @param width_ Application window width
     * @param height_ Application window height
     */
    void Resize(int width_, int height_) override;
    /**
     * @brief Release Skybox buffers and textures
     */
    void ReleaseFbo() override;
    /**
     * @brief Bind Skybox frame buffer
     */
    void BindFbo() const override;
    /**
     * @brief Get Skybox texture
     * @return Skybox texture buffer id (uint)
     */
    RS_N_D unsigned int GetSkyboxTexture() const { return skybox_texture; }
  private:
    unsigned int skybox_frame_buffer = 0; ///< Skybox frame buffer id
    unsigned int skybox_texture = 0; ///< Skybox texture buffer id
  };
} // namespace _RS_Internal


#endif
