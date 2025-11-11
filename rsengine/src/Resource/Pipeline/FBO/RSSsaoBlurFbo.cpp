#include "pch.h"
#include "RSSsaoBlurFbo.h"

namespace _RS_Internal
{
  void RSSsaoBlurFbo::CreateFbo(const int width_, const int height_)
  {
    // SSAO Blur Frame buffer
    glGenFramebuffers(1, &ssao_blur_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_frame_buffer);

    // SSAO Blur texture
    glGenTextures(1, &ssao_blur_texture);
    glBindTexture(GL_TEXTURE_2D, ssao_blur_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_blur_texture, 0);
    if (!CheckFrameBufferStatus())
      RS_WARN("SSAO Blur Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void RSSsaoBlurFbo::Resize(const int width_, const int height_)
  {
    // SSAO Blur Frame buffer
    glBindTexture(GL_TEXTURE_2D, ssao_blur_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RSSsaoBlurFbo::ReleaseFbo()
  {
    glDeleteFramebuffers(1, &ssao_blur_frame_buffer);
    glDeleteTextures(1, &ssao_blur_texture);
  }

  void RSSsaoBlurFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_frame_buffer);
  }
  
} // namespace _RS_Internal
