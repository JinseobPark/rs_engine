#include "pch.h"
#include "RSTextButtonWidget.h"

#include <algorithm>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"

namespace RS_Widget
{
  using RS_Shader::RSShaderNames;
  RSTextButtonWidget::RSTextButtonWidget(const std::wstring& text_)
  {
    SetText(text_);
    b_flip_y = 0;
    m_visibility = RSVisibility::HIT_VISIBLE;
  }

  RSTextButtonWidget::~RSTextButtonWidget()
  {
    m_callback = nullptr;
  }

  void RSTextButtonWidget::Initialize()
  {
    RSButtonWidget::Initialize();
    RenderTextToTexture();
  }

  void RSTextButtonWidget::Shutdown()
  {
    if (text_vao_)
      glDeleteVertexArrays(1, &text_vao_);
    if (text_vbo_)
      glDeleteBuffers(1, &text_vbo_);
    if (text_texture_vao)
      glDeleteVertexArrays(1, &text_texture_vao);
    if (text_texture_vbo)
      glDeleteBuffers(1, &text_texture_vbo);

    if (m_default_text_texture)
      glDeleteTextures(1, &m_default_text_texture);
    if (m_func_text_texture)
      glDeleteTextures(1, &m_func_text_texture);
    if (m_text_texture)
      glDeleteTextures(1, &m_text_texture);

    text_vao_ = 0;
    text_vbo_ = 0;
    text_texture_vao = 0;
    text_texture_vbo = 0;
  }

  void RSTextButtonWidget::Update(const float dt)
  {
    RSButtonWidget::Update(dt);
    m_texture_id = is_hovered ? m_func_text_texture : m_default_text_texture;
  }


  void RSTextButtonWidget::RenderTextToTexture()
  {
    if (m_text.empty())
      return;
    DeleteCustomTextures();

    CreateTextBuffer();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CreateTextTexture();

    CalculateTextPosition();

    CreateDefaultTexture();

    CreateFuncTexture();

    glDisable(GL_BLEND);
    DeleteTextBuffer();
  }

  void RSTextButtonWidget::SetText(const std::wstring& text_)
  {
    m_text = text_;
    //RenderTextToTexture();
  }

  void RSTextButtonWidget::SetTextSize(const float size_)
  {
    m_text_size = size_;
    //RenderTextToTexture();
  }

  void RSTextButtonWidget::SetPosition(const glm::vec2 pos_)
  {
    m_transform.SetPosition(glm::vec3(pos_.x, pos_.y, 0.0f));
    m_origin_position = m_transform.GetPosition();
  }

  void RSTextButtonWidget::SetPosition(const float pos_x, const float pos_y)
  {
    m_transform.SetPosition(glm::vec3(pos_x, pos_y, 0.0f));
    m_origin_position = m_transform.GetPosition();
  }

  void RSTextButtonWidget::SetTextAlignment(const RSTextAlignment text_align_)
  {
    m_text_alignment = text_align_;
    //RenderTextToTexture();
  }

  void RSTextButtonWidget::Resize()
  {
    m_dirty = true;
  }

  void RSTextButtonWidget::SetDefaultTexture(const GLuint texture_id_)
  {
    m_default_texture_id = texture_id_;
    //RenderTextToTexture();
  }

  void RSTextButtonWidget::SetFuncTexture(const GLuint texture_id_)
  {
    m_func_texture_id = texture_id_;
    //RenderTextToTexture();
  }

  void RSTextButtonWidget::DeleteCustomTextures()
  {
    if (m_default_text_texture)
      glDeleteTextures(1, &m_default_text_texture);

    if (m_func_text_texture)
      glDeleteTextures(1, &m_func_text_texture);

    if (m_text_texture)
      glDeleteTextures(1, &m_text_texture);
  }

  void RSTextButtonWidget::CreateTextBuffer()
  {
    // Create VAO, VBO
    glGenVertexArrays(1, &text_vao_);
    glGenBuffers(1, &text_vbo_);
    glBindVertexArray(text_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void RSTextButtonWidget::DeleteTextBuffer()
  {
    glDeleteVertexArrays(1, &text_vao_);
    text_vao_ = 0;
    glDeleteBuffers(1, &text_vbo_);
    text_vbo_ = 0;
  }

  void RSTextButtonWidget::CreateTextTexture()
  {
    const RSResourceManager* p_resource_manager = RSResourceManager::GetInstance();
    // Calculate text width and height
    float max_height = 0.0f;

    texture_width = 0;
    texture_height = 0;


    std::wstring::const_iterator c;
    const auto characters_map = p_resource_manager->GetTextureManager()->GetCharacters();
    const float text_anti_aliasing = p_resource_manager->GetTextureManager()->GetTextAntiAliasing();

    // Calculate RSTexture Width and Height
    for (c = m_text.begin(); c != m_text.end(); ++c)
    {
      auto ch_it = characters_map->find(*c);
      if (ch_it == characters_map->end())
        continue;
      auto [texture_id, size, bearing, advance] = ch_it->second;
      texture_width += static_cast<float>(advance >> 6) * m_text_size;
      const float height = static_cast<float>(size.y);
      const float ch_descent = static_cast<float>(size.y) - static_cast<float>(bearing.y);
      max_height = std::max(height, max_height);
      m_descent = std::max(ch_descent, m_descent);
    }
    texture_height = max_height * m_text_size;
    m_descent = m_descent * m_text_size;

    // Add anti-aliasing
    texture_width += text_anti_aliasing;
    texture_height += text_anti_aliasing;

    // Set Scale
    //SetSize(texture_width, texture_height);
    //RSWidgetBase::SetPosition(m_origin_position + glm::vec3(texture_width, 0.0f, 0.0f) /
    //                          static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth()) * 2.0f);

    texture_width *= text_anti_aliasing;
    texture_height *= text_anti_aliasing;
    m_anti_text_size = m_text_size * text_anti_aliasing;
    m_descent *= text_anti_aliasing;


  }

  void RSTextButtonWidget::DrawDefaultTexture()
  {
    const RSResourceManager* p_resource_manager = RSResourceManager::GetInstance();
    // Draw Default texture
    p_resource_manager->GetShaderManager()->Use(RSShaderNames::HUD_TEXTURE);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "model", m_transform.GetTransformMatrix());
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "projection",
                                                    glm::ortho(-GetSize().x, GetSize().x, -GetSize().y,
                                                      GetSize().y));
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "textColor", m_color);
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "flipY", !b_flip_y);

    glBindTexture(GL_TEXTURE_2D, m_default_texture_id);
    RSResourceManager::GetInstance()->GetBufferManager()->DrawQuad();
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RSTextButtonWidget::DrawFuncTexture()
  {
    const RSResourceManager* p_resource_manager = RSResourceManager::GetInstance();
    // Draw Default texture
    p_resource_manager->GetShaderManager()->Use(RSShaderNames::HUD_TEXTURE);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "model", m_transform.GetTransformMatrix());
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "projection",
                                                    glm::ortho(-GetSize().x, GetSize().x, -GetSize().y,
                                                      GetSize().y));
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "textColor", m_color);
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "flipY", !b_flip_y);

    glBindTexture(GL_TEXTURE_2D, m_func_texture_id);
    RSResourceManager::GetInstance()->GetBufferManager()->DrawQuad();
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void RSTextButtonWidget::DrawTextTexture()
  {
  }

  void RSTextButtonWidget::CreateDefaultTexture()
  {
    const RSResourceManager* p_resource_manager = RSResourceManager::GetInstance();
    std::wstring::const_iterator c;
    const auto characters_map = p_resource_manager->GetTextureManager()->GetCharacters();
    RS_Shader::RSShaderManager* p_sm = p_resource_manager->GetShaderManager();

    // Create a texture from text
    glGenTextures(1, &m_default_text_texture);
    glBindTexture(GL_TEXTURE_2D, m_default_text_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_transform.GetScale().x),
                 static_cast<int>(m_transform.GetScale().y), 0,GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    GLuint temp_frame_buffer;
    glGenFramebuffers(1, &temp_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, temp_frame_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_default_text_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      RS_WARN("Frame buffer not complete!");
      return;
    }


    glViewport(0, 0, static_cast<int>(m_transform.GetScale().x), static_cast<int>(m_transform.GetScale().y));
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    DrawDefaultTexture();
    
    glViewport(m_text_position.x, m_text_position.y, texture_width, texture_height);

    p_sm->Use(RSShaderNames::HUD_TEXT);
    p_sm->SetData(RSShaderNames::HUD_TEXT, "textColor", m_text_color);
    p_sm->SetData(RSShaderNames::HUD_TEXT, "projection",
      glm::ortho(0.0f, texture_width,
        -0.0f, texture_height));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_vao_);

    float x = 0.0f;
    for (c = m_text.begin(); c != m_text.end(); ++c)
    {
      auto ch_it = characters_map->find(*c);
      if (ch_it == characters_map->end())
        continue;
      auto [texture_id, size, bearing, advance] = ch_it->second;

      const float xpos = x + static_cast<float>(bearing.x) * m_anti_text_size;
      const float ypos = m_descent - static_cast<float>(size.y - bearing.y) * m_anti_text_size;
      const float w = static_cast<float>(size.x) * m_anti_text_size;
      const float h = static_cast<float>(size.y) * m_anti_text_size;
      const float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
      };

      glBindTexture(GL_TEXTURE_2D, texture_id);
      glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      x += static_cast<float>(advance >> 6) * m_anti_text_size;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &temp_frame_buffer);

  }

  void RSTextButtonWidget::CreateFuncTexture()
  {
    const RSResourceManager* p_resource_manager = RSResourceManager::GetInstance();
    std::wstring::const_iterator c;
    const auto characters_map = p_resource_manager->GetTextureManager()->GetCharacters();
    RS_Shader::RSShaderManager* p_sm = p_resource_manager->GetShaderManager();
    // Create a texture from text
    glGenTextures(1, &m_func_text_texture);
    glBindTexture(GL_TEXTURE_2D, m_func_text_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(m_transform.GetScale().x),
                 static_cast<int>(m_transform.GetScale().y), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    GLuint temp_frame_buffer;
    glGenFramebuffers(1, &temp_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, temp_frame_buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_func_text_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      RS_WARN("Frame buffer not complete!");
      return;
    }


    glViewport(0, 0, static_cast<int>(m_transform.GetScale().x), static_cast<int>(m_transform.GetScale().y));
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    DrawFuncTexture();
    glViewport(static_cast<int>(m_text_position.x), static_cast<int>(m_text_position.y), static_cast<int>(texture_width), static_cast<int>(texture_height));

    p_sm->Use(RSShaderNames::HUD_TEXT);
    p_sm->SetData(RSShaderNames::HUD_TEXT, "textColor", m_text_color);
    p_sm->SetData(RSShaderNames::HUD_TEXT, "projection", 
      glm::ortho(0.0f, texture_width,
      -0.0f, texture_height));


    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_vao_);

    float x = 0.0f;
    for (c = m_text.begin(); c != m_text.end(); ++c)
    {
      auto ch_it = characters_map->find(*c);
      if (ch_it == characters_map->end())
        continue;
      auto [texture_id, size, bearing, advance] = ch_it->second;

      const float xpos = x + static_cast<float>(bearing.x) * m_anti_text_size;
      const float ypos = m_descent - static_cast<float>(size.y - bearing.y) * m_anti_text_size;
      const float w = static_cast<float>(size.x) * m_anti_text_size;
      const float h = static_cast<float>(size.y) * m_anti_text_size;
      const float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
      };

      glBindTexture(GL_TEXTURE_2D, texture_id);
      glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      x += static_cast<float>(advance >> 6) * m_anti_text_size;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &temp_frame_buffer);
  }

  void RSTextButtonWidget::CalculateTextPosition()
  {
    //Calculate text position by alignment

    // When text alignment is left, text position's width is 0, height is middle of the texture height
    // Formula : (0, (btn height - height) / 2)

    // When text alignment is middle, text position's width is (btn width - width) / 2, height is middle of the texture height
    // Formula : ((btn width - width) / 2, (btn height - height) / 2)

    // When text alignment is right, text position's width is (btn width - width), height is middle of the texture height
    // Formula : (btn width - width, (btn height - height) / 2)

    // Common : height is over btn height, Set widget height to text height.
    // Also when width is over btn width, set widget width to text width.

    if (texture_width >= GetSize().x)
      SetSize(texture_width, GetSize().y);

    if (texture_height >= GetSize().y)
      SetSize(GetSize().x, texture_height);


    switch (m_text_alignment)
    {
    case RSTextAlignment::LEFT:
      m_text_position = (glm::vec3(0.0, (GetSize().y - texture_height) / 2.0f, 0.0f));
      break;

    case RSTextAlignment::MIDDLE:
      m_text_position = (glm::vec3((GetSize().x - texture_width) * 0.5f,
                                   (GetSize().y - texture_height) *0.5f, 0.0f));
      break;

    case RSTextAlignment::RIGHT:
      m_text_position = (glm::vec3((GetSize().x - texture_width),
                                   (GetSize().y - texture_height) / 2.0f, 0.0f));
      break;
    }

  }



}