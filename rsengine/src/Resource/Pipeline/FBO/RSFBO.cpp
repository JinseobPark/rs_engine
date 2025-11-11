#include "pch.h"
#include "RSFBO.h"

namespace _RS_Internal
{
  
void RSFBO::UnbindFbo()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool RSFBO::CheckFrameBufferStatus()
{
  switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
  {
  case GL_FRAMEBUFFER_COMPLETE:
    RS_MSG("Framebuffer is complete");
    return true;

  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    RS_WARN("Framebuffer is incomplete: Attachment is NOT complete");
    return false;

  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    RS_WARN("Framebuffer is incomplete: No image is attached to FBO");
    return false;

  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    RS_WARN("Framebuffer is incomplete: Draw buffer");
    return false;

  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    RS_WARN("Framebuffer is incomplete: Read buffer");
    return false;

  case GL_FRAMEBUFFER_UNSUPPORTED:
    RS_WARN("Framebuffer is incomplete: Unsupported by FBO implementation");
    return false;

  default:
    RS_WARN("Framebuffer is incomplete: Unknown error");
    return false;
    
  }

}

} // namespace _RS_Internal
