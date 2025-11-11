#include "pch.h"
#include "RSDeferredFbo.h"
#include "Systems/Application/RSApplication.h"

namespace _RS_Internal
{
  void RSDeferredFbo::CreateFbo(const int width_, const int height_)
  {
    // Deferred Frame buffer
    glGenFramebuffers(1, &deferred_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, deferred_frame_buffer);
    // Position texture
    glGenTextures(1, &deferred_position_texture);
    glBindTexture(GL_TEXTURE_2D, deferred_position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, deferred_position_texture, 0);
    // Normal texture
    glGenTextures(1, &deferred_normal_texture);
    glBindTexture(GL_TEXTURE_2D, deferred_normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferred_normal_texture, 0);
    // Albedo texture
    glGenTextures(1, &deferred_albedo_texture);
    glBindTexture(GL_TEXTURE_2D, deferred_albedo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferred_albedo_texture, 0);

    ////Depth texture
    //glGenTextures(1, &deferred_depth_texture);
    //glBindTexture(GL_TEXTURE_2D, deferred_depth_texture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, deferred_depth_texture, 0);

    // Fluid Deferred Position texture
    glGenTextures(1, &fluid_deferred_position_texture);
    glBindTexture(GL_TEXTURE_2D, fluid_deferred_position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, fluid_deferred_position_texture, 0);
    // Fluid Deferred Normal texture
    glGenTextures(1, &fluid_deferred_normal_texture);
    glBindTexture(GL_TEXTURE_2D, fluid_deferred_normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, fluid_deferred_normal_texture, 0);
    // Fluid Deferred Albedo texture
    glGenTextures(1, &fluid_deferred_albedo_texture);
    glBindTexture(GL_TEXTURE_2D, fluid_deferred_albedo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, fluid_deferred_albedo_texture, 0);

    // Custom Depth texture. Write the opaque and fluid depth to each channel R,G. later it will increase to 4 channels
    glGenTextures(1, &custom_deferred_depth_texture);
    glBindTexture(GL_TEXTURE_2D, custom_deferred_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, custom_deferred_depth_texture, 0);

    // Post Process Mask texture
    glGenTextures(1, &m_custom_mask_texture);
    glBindTexture(GL_TEXTURE_2D, m_custom_mask_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width_, height_, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, m_custom_mask_texture, 0);

    // General Depth texture
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);


    // Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    constexpr unsigned int attachments[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
                                              GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
    glDrawBuffers(8, attachments);
    if (!CheckFrameBufferStatus())
      RS_WARN("Deferred Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void RSDeferredFbo::Resize(const int width_, const int height_)
  {
    // Deferred Frame buffer
    glBindTexture(GL_TEXTURE_2D, deferred_position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, deferred_normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, deferred_albedo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, fluid_deferred_position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, fluid_deferred_normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, fluid_deferred_albedo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, custom_deferred_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_custom_mask_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width_, height_, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  }

  void RSDeferredFbo::ReleaseFbo()
  {
    glDeleteFramebuffers(1, &deferred_frame_buffer);
    glDeleteTextures(1, &deferred_position_texture);
    glDeleteTextures(1, &deferred_normal_texture);
    glDeleteTextures(1, &deferred_albedo_texture);
    glDeleteTextures(1, &fluid_deferred_position_texture);
    glDeleteTextures(1, &fluid_deferred_normal_texture);
    glDeleteTextures(1, &fluid_deferred_albedo_texture);
    glDeleteTextures(1, &custom_deferred_depth_texture);
    glDeleteTextures(1, &depth_texture);
    glDeleteTextures(1, &m_custom_mask_texture);


  }
   
  void RSDeferredFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, deferred_frame_buffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

}
