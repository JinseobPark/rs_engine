#include "pch.h"
#include "RSSsrBlurFbo.h"

namespace _RS_Internal
{
  void RSSsrBlurFbo::CreateFbo(const int width_, const int height_)
  {
    // SSR Blur Frame buffer
    glGenFramebuffers(1, &ssr_blur_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ssr_blur_frame_buffer);

    // SSR Blur texture
    glGenTextures(1, &ssr_blur_texture);
    glBindTexture(GL_TEXTURE_2D, ssr_blur_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssr_blur_texture, 0);
    if (!CheckFrameBufferStatus())
      RS_WARN("SSR Blur Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
  }

  void RSSsrBlurFbo::Resize(const int width_, const int height_)
  {
    // SSR Blur Frame buffer
    glBindTexture(GL_TEXTURE_2D, ssr_blur_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RSSsrBlurFbo::ReleaseFbo()
  {
    glDeleteFramebuffers(1, &ssr_blur_frame_buffer);
    glDeleteTextures(1, &ssr_blur_texture);
  }

  void RSSsrBlurFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, ssr_blur_frame_buffer);
  }

}