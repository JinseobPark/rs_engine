/*********************************************************************
 * \file   RSSSAO.h
 * \brief  SSAO class for rendering system.
 * Screen space ambient occlusion class for rendering system.
 * \author jinseob Park
 * \date   2025/03/12
 *********************************************************************/
#ifndef RS_SSAO_H_
#define RS_SSAO_H_

#include "RSFBO.h"

/**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
  */
namespace _RS_Internal
{
  
 /**
  * @brief SSAO class
  * This class is for screen space ambient occlusion.
  */
class RSSsaoFbo : public RSFBO
{
public:
  RSSsaoFbo()           = default;
  ~RSSsaoFbo() override = default;


  /**
   * @brief Create SSAO buffers and textures include noise texture
   */
  void CreateFbo(const int width_, const int height_) override;

  /**
   * @brief Resize SSAO texture and Blur texture
   * @param width_ Application window width
   * @param height_ Application window height
   */
  void Resize(int width_, int height_) override;

  /**
   * @brief Release SSAO buffers and textures include noise texture
   */
  void ReleaseFbo() override;

  /**
   * @brief Bind SSAO frame buffer
   */
  void BindFbo() const override;

  /**
   * @brief Get SSAO texture
   * @return SSAO texture buffer id (uint)
   */
  unsigned int GetSsaoTexture() const { return ssao_texture; }

  /**
   * @brief Get Noise texture
   * @return Noise texture buffer id (uint)
   */
  unsigned int GetNoiseTexture() const { return noise_texture; }

  /**
   * @brief Get SSAO kernel
   * @return SSAO kernel (vector of glm::vec3)
   */
  const std::vector<glm::vec3>* GetKernel() const { return &ssao_kernel; }

  /**
   * @brief Check SSAO kernel is dirty or not
   * @return Dirty flag
   */
  bool IsDirty() const { return m_dirty; }

  /**
   * @brief Set dirty flag for SSAO kernel update
   * @param dirty_ Dirty flag
   */
  void SetDirty(const bool dirty_) { m_dirty = dirty_; }

  /**
   * @brief Get SSAO radius
   * @return SSAO radius (float)
   */
  float GetRadius() const { return m_radius; }

  /**
   * @brief Set SSAO radius
   * @param radius_ SSAO radius (float)
   */
  void SetRadius(const float radius_) { m_radius = radius_; }

  /**
   * @brief Get SSAO bias
   * @return SSAO bias (float)
   */
  float GetBias() const { return m_bias; }

  /**
   * @brief Set SSAO bias
   * @param bias_ SSAO bias (float)
   */
  void SetBias(const float bias_) { m_bias = bias_; }

private:
  unsigned int ssao_frame_buffer = 0; ///< SSAO frame buffer id

  unsigned int ssao_texture = 0; ///< SSAO texture id

  unsigned int noise_texture = 0; ///< Noise texture buffer id

  std::vector<glm::vec3> ssao_kernel; ///< SSAO kernel

  bool m_dirty = true; ///< Dirty flag for SSAO kernel

  float m_radius = 0.5f; ///< SSAO radius
  float m_bias = 0.025f; ///< SSAO bias
};

} // namespace _RS_Internal

#endif
