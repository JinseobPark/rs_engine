#include "pch.h"
#include "RSPickingFbo.h"
#include "Systems/Application/RSApplication.h"

namespace _RS_Internal
{
  void RSPickingFbo::CreateFbo(const int width_, const int height_)
  {
		// this is for Picking frame buffer. So is managed by object id
		glGenFramebuffers(1, &picking_frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, picking_frame_buffer);

		glGenTextures(1, &picking_texture);
		glBindTexture(GL_TEXTURE_2D, picking_texture);
		// 32bit unsigned int ID
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width_, height_, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		// set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// attach texture to frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture, 0);

		//create a render buffer object for depth and stencil attachment (we won't be sampling these)
		glGenRenderbuffers(1, &picking_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, picking_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, picking_rbo);

		if (!CheckFrameBufferStatus())
      RS_WARN("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void RSPickingFbo::Resize(const int width_, const int height_)
  {
    // Picking RSTexture resize
    glBindTexture(GL_TEXTURE_2D, picking_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width_, height_, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // picking_rbo resize
    glBindRenderbuffer(GL_RENDERBUFFER, picking_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }

  void RSPickingFbo::ReleaseFbo()
  {
    glDeleteFramebuffers(1, &picking_frame_buffer);
    glDeleteTextures(1, &picking_texture);
    glDeleteRenderbuffers(1, &picking_rbo);
  }

  void RSPickingFbo::BindFbo() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, picking_frame_buffer);
    GLuint clear_value = 0;
    glClearBufferuiv(GL_COLOR, 0, &clear_value);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

} // namespace _RS_Internal
