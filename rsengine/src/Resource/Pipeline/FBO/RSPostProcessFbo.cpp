#include "pch.h"
#include "RSPostProcessFbo.h"
#include "Systems/Application/RSApplication.h"

namespace _RS_Internal
{

void RSPostProcessFbo::CreateFbo(const int width_, const int height_)
{
  // Create framebuffer
  glGenFramebuffers(1, &m_frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

  // Create color texture (RGB16F for HDR support)
  glGenTextures(1, &m_color_texture);
  glBindTexture(GL_TEXTURE_2D, m_color_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width_, height_, 0, GL_RGB, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Attach texture to framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

  if (!CheckFrameBufferStatus())
    RS_WARN("Post-process Framebuffer is not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void RSPostProcessFbo::Resize(const int width_, const int height_)
{
  glBindTexture(GL_TEXTURE_2D, m_color_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width_, height_, 0, GL_RGB, GL_FLOAT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void RSPostProcessFbo::ReleaseFbo()
{
  if (m_frame_buffer != 0)
  {
    glDeleteFramebuffers(1, &m_frame_buffer);
    m_frame_buffer = 0;
  }
  if (m_color_texture != 0)
  {
    glDeleteTextures(1, &m_color_texture);
    m_color_texture = 0;
  }
}

void RSPostProcessFbo::BindFbo() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
}

} // namespace _RS_Internal
