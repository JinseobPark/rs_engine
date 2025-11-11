#include "pch.h"
#include "RSSsaoFbo.h"
#include "Systems/Application/RSApplication.h"
#include <random>

namespace _RS_Internal
{
void RSSsaoFbo::CreateFbo(const int width_, const int height_)
{
  // SSAO Frame buffer
  glGenFramebuffers(1, &ssao_frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, ssao_frame_buffer);

  // SSAO texture
  glGenTextures(1, &ssao_texture);
  glBindTexture(GL_TEXTURE_2D, ssao_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_texture, 0);
  if (!CheckFrameBufferStatus())
    RS_WARN("SSAO Framebuffer is not complete!");

  // Generate sample kernel
  std::uniform_real_distribution<float> random_floats(0.0f, 1.0f);
  std::default_random_engine generator;

  ssao_kernel.reserve(64);
  for (unsigned int i = 0; i < 64; ++i)
  {
    glm::vec3 sample(
      random_floats(generator) * 2.0f - 1.0f,
      random_floats(generator) * 2.0f - 1.0f,
      random_floats(generator)
    );
    sample = glm::normalize(sample);
    sample *= random_floats(generator);
    float scale = static_cast<float>(i) / 64.0f;

    scale = 0.1f + (scale * scale) * (1.0f - 0.1f); // Lerp
    sample *= scale;
    ssao_kernel.push_back(sample);
  }

  // Generate noise data for SSAO
  std::vector<glm::vec3> ssao_noise;
  ssao_noise.reserve(16);
  for (unsigned int i = 0; i < 16; ++i)
  {
    glm::vec3 noise(
      random_floats(generator) * 2.0f - 1.0f,
      random_floats(generator) * 2.0f - 1.0f,
      0.0f
    );
    ssao_noise.push_back(noise);
  }

  // Noise texture
  glGenTextures(1, &noise_texture);
  glBindTexture(GL_TEXTURE_2D, noise_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssao_noise.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void RSSsaoFbo::Resize(const int width_, const int height_)
{
  // SSAO Frame buffer
  glBindTexture(GL_TEXTURE_2D, ssao_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

}

void RSSsaoFbo::ReleaseFbo()
{
  // SSAO Frame buffers and textures
  glDeleteFramebuffers(1, &ssao_frame_buffer);
  ssao_frame_buffer = 0;
  glDeleteTextures(1, &ssao_texture);
  ssao_texture = 0;
  glDeleteTextures(1, &noise_texture);
  noise_texture = 0;
}

void RSSsaoFbo::BindFbo() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, ssao_frame_buffer);
}


}
