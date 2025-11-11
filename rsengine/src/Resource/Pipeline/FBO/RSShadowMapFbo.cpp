#include "pch.h"
#include "RSShadowMapFbo.h"

namespace _RS_Internal
{
  void RSShadowMapFbo::CreateFbo(const int width_, const int height_)
  {
		glGenFramebuffers(1, &shadow_map_frame_buffer);

		glGenTextures(1, &shadow_map_texture);
		glBindTexture(GL_TEXTURE_2D, shadow_map_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		constexpr float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);


		glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_frame_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void RSShadowMapFbo::Resize(int width_, int height_)
  {
    // Not resize the shadow map texture
  }

  void RSShadowMapFbo::ReleaseFbo()
  {
    glDeleteFramebuffers(1, &shadow_map_frame_buffer);
    glDeleteTextures(1, &shadow_map_texture);
  }

  void RSShadowMapFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_frame_buffer);
    glViewport(0, 0, shadow_width, shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  void RSShadowMapFbo::ClearShadowMap()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_frame_buffer);
    glViewport(0, 0, shadow_width, shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

}   // namespace _RS_Internal
