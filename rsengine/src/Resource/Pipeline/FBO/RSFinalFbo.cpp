#include "pch.h"
#include "RSFinalFbo.h"
#include "Systems/Application/RSApplication.h"

namespace _RS_Internal
{
  void RSFinalFbo::CreateFbo(const int width_, const int height_)
  {
    glGenFramebuffers(1, &final_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, final_frame_buffer);
    glGenTextures(1, &final_texture_buffer);
    glBindTexture(GL_TEXTURE_2D, final_texture_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, final_texture_buffer, 0);

    // create a render buffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &final_render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, final_render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, final_render_buffer);

    if (!CheckFrameBufferStatus())
      RS_WARN("Framebuffer not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

  }
  void RSFinalFbo::Resize(const int width_, const int height_)
  {
    glBindTexture(GL_TEXTURE_2D, final_texture_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, final_render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  void RSFinalFbo::ReleaseFbo()
  {
    glDeleteFramebuffers(1, &final_frame_buffer);
    glDeleteTextures(1, &final_texture_buffer);
    glDeleteRenderbuffers(1, &final_render_buffer);
  }
  void RSFinalFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, final_frame_buffer);
  }
}