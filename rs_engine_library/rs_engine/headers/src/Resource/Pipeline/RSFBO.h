/*****************************************************************//**
 * \file   RSFBO.h
 * \brief  RS engine Frame buffer object class for rendering system.
 * 
 * \author Jinseob Park
 * \date   March 2025
 *********************************************************************/
#ifndef _RSFBO_H_
#define _RSFBO_H_

#include <GL/glew.h>

class RSFBO
{
public:
  RSFBO() = default;
  ~RSFBO() = default;

  /**
   * @brief Create Frame buffer object
   */
  void CreateFbo();

  /**
   * @brief Bind Frame buffer object
   */
  void BindFbo();

  /**
   * @brief Unbind Frame buffer object
   */
  void UnbindFbo();

  /**
   * @brief Add buffer to frame buffer object
   * @param buffer_id_ buffer id
   * @param n index of buffer
   */
  void AddBufferToFbo(GLuint& buffer_id_, unsigned int n);

  /**
   * @brief Resize frame buffer object
   * @param width_ width
   * @param height_ height
   */
  void ResizeFbo(int width_, int height_);

  /**
   * @brief Release buffers (Delete buffers)
   */
  void ReleaseBuffers();

  /**
   * @brief Get Frame buffer object
   * @return frame buffer object id
   */
  GLuint GetFbo() const { return m_fbo; }
private:

  /**
   * @brief Check frame buffer status
   * @return true if frame buffer is complete
   */
  bool CheckFrameBufferStatus();

  GLuint m_fbo = 0; ///< frame buffer object
  GLuint m_rbo = 0; ///< depth buffer
};

#endif // !_RSFBO_H_
