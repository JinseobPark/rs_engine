#include "pch.h"
#include "RSHudFbo.h"
#include "Systems/Application/RSApplication.h"

namespace _RS_Internal
{
  void RSHudFbo::CreateFbo(const int width_, const int height_)
  {
    glGenFramebuffers(1, &hud_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, hud_frame_buffer);

    glGenTextures(1, &hud_texture);
    glBindTexture(GL_TEXTURE_2D, hud_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hud_texture, 0);
    if (!CheckFrameBufferStatus())
      RS_WARN("HUD Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  void RSHudFbo::Resize(const int width_, const int height_)
  {
    glBindTexture(GL_TEXTURE_2D, hud_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  void RSHudFbo::ReleaseFbo()
  {
    glDeleteFramebuffers(1, &hud_frame_buffer);
    glDeleteTextures(1, &hud_texture);
  }
  void RSHudFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, hud_frame_buffer);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    ///glViewport(0, 0, RS_Application::mRSAPPLICATION->GetWidth(), RS_Application::mRSAPPLICATION->GetHeight()); // Consider to remove this code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}   // namespace _RS_Internal
