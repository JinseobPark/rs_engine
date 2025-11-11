#include "pch.h"
#include "RSSsrFbo.h"

namespace _RS_Internal
{
  void RSSsrFbo::CreateFbo(const int width_, const int height_)
  {
    // SSR Frame buffer
    glGenFramebuffers(1, &ssr_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ssr_frame_buffer);

    // SSR texture
    glGenTextures(1, &ssr_texture);
    glBindTexture(GL_TEXTURE_2D, ssr_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssr_texture, 0);
    if (!CheckFrameBufferStatus())
      RS_WARN("SSR Framebuffer is not complete!");
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void RSSsrFbo::Resize(const int width_, const int height_)
  {
    // SSR texture
    glBindTexture(GL_TEXTURE_2D, ssr_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RSSsrFbo::ReleaseFbo()
  {
    glDeleteTextures(1, &ssr_texture);
    glDeleteFramebuffers(1, &ssr_frame_buffer);
  }

  void RSSsrFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, ssr_frame_buffer);
  }

}