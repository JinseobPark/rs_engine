#include "pch.h"
#include "RSTextWidget.h"

#include <algorithm>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"

namespace RS_Widget
{
	using RS_Shader::RSShaderNames;
	RSTextWidget::RSTextWidget(const RSTextStaticDynamic type_)
	{
		text_type_ = type_;
		b_flip_y = 0;
		m_visibility = RSVisibility::NONE_HIT_VISIBLE;
	}

	RSTextWidget::~RSTextWidget() = default;

	void RSTextWidget::Initialize()
	{
		// if text_type is dynamic, create text VAO, VBO. Because text will be changed dynamically
		if (text_type_ == RSTextStaticDynamic::DYNAMIC)
		{
			// Create VAO, VBO
			glGenVertexArrays(1, &text_vao_);
			glGenBuffers(1, &text_vbo_);
			glBindVertexArray(text_vao_);
			glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		origin_position = m_transform.GetPosition();
	}

	void RSTextWidget::Shutdown()
	{
		if (text_vao_)
			glDeleteVertexArrays(1, &text_vao_);
		if (text_vbo_)
			glDeleteBuffers(1, &text_vbo_);
		if(text_texture_vao)
			glDeleteVertexArrays(1, &text_texture_vao);
		if (text_texture_vbo)
			glDeleteBuffers(1, &text_texture_vbo);

		text_vao_ = 0;
		text_vbo_ = 0;
		text_texture_vao = 0;
		text_texture_vbo = 0;
	}

	void RSTextWidget::Update(float dt)
	{
		if (dirty_)
		{
			RenderTextToTexture();
			dirty_ = false;
		}
	}

	void RSTextWidget::Draw()
	{
		// Draw text. BUT NOT USED now
#if 0
		if (m_texture_id)
		{
			// Draw text
			glBindVertexArray(text_texture_VAO); // Quad VAO
			glBindTexture(GL_TEXTURE_2D, m_texture_id);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
#endif
	}

	void RSTextWidget::RenderTextToTexture() {
		if (m_text.empty())
			return;
		const RSResourceManager* p_resource_manager = RSResourceManager::GetInstance();
		// Calculate text width and height
		float max_height = 0.0f, descent = 0.0f;

		// Initialize texture ID
		if (m_texture_id)
			glDeleteTextures(1, &m_texture_id);

		// if type is static, create text VAO, VBO. Because text will be delete after rendering
		if (text_type_ == RSTextStaticDynamic::STATIC)
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
			texture_width += static_cast<float>(advance >> 6) * text_size;
			const float height     = static_cast<float>(size.y);
			const float ch_descent = static_cast<float>(size.y) - static_cast<float>(bearing.y);
      max_height             = std::max(height, max_height);
      descent                = std::max(ch_descent, descent);
    }
		texture_height = max_height * text_size;
		descent = descent * text_size;

		// Add anti-aliasing
		texture_width += text_anti_aliasing;
		texture_height += text_anti_aliasing;

		// Set Scale
		SetSize(texture_width, texture_height);
		RSWidgetBase::SetPosition(origin_position + glm::vec3(texture_width, 0.0f, 0.0f) / 
			                        static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth()) * 2.0f);

		texture_width *= text_anti_aliasing;
		texture_height *= text_anti_aliasing;
		const float anti_text_size = text_size * text_anti_aliasing;
		descent *= text_anti_aliasing;

		// Create a texture from text
		glGenTextures(1, &m_texture_id);
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(texture_width), static_cast<int>(texture_height), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);



		GLuint temp_frame_buffer;
		glGenFramebuffers(1, &temp_frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, temp_frame_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			RS_WARN("Frame buffer not complete!");
			return;
		}

			glViewport(0, 0, static_cast<int>(texture_width), static_cast<int>(texture_height));
			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT);

			p_resource_manager->GetShaderManager()->Use(RSShaderNames::HUD_TEXT);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXT, "textColor", m_text_color);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXT, "projection",
                                                      glm::ortho(0.0f, texture_width, 0.0f, texture_height));

			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(text_vao_);

			float x = 0.0f;
			for (c = m_text.begin(); c != m_text.end(); ++c)
			{
				auto ch_it = characters_map->find(*c);
				if (ch_it == characters_map->end())
					continue;
				auto [texture_id, size, bearing, advance] = ch_it->second;

				const float xpos = x + static_cast<float>(bearing.x) * anti_text_size;
				const float ypos = descent - static_cast<float>(size.y - bearing.y) * anti_text_size;
				const float w = static_cast<float>(size.x) * anti_text_size;
				const float h = static_cast<float>(size.y) * anti_text_size;
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

				x += static_cast<float>(advance >> 6) * anti_text_size;
			}

			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &temp_frame_buffer);

		// If text type is static, delete VAO, VBO
		if (text_type_ == RSTextStaticDynamic::STATIC)
		{
			glDeleteVertexArrays(1, &text_vao_);
			text_vao_ = 0;
			glDeleteBuffers(1, &text_vbo_);
			text_vbo_ = 0;
		}
	}

	void RSTextWidget::SetText(const std::wstring& text_)
	{
		m_text = text_;
		dirty_ = true;
	}

	void RSTextWidget::SetTextSize(const float size_)
	{
		text_size = size_;
		dirty_ = true;
	}

	void RSTextWidget::SetPosition(const glm::vec2 pos_)
	{
		m_transform.SetPosition(glm::vec3(pos_.x, pos_.y, 0.0f));
		origin_position = m_transform.GetPosition();
		dirty_ = true;
	}

	void RSTextWidget::SetPosition(const float pos_x, const float pos_y)
	{
		m_transform.SetPosition(glm::vec3(pos_x, pos_y, 0.0f));
		origin_position = m_transform.GetPosition();
		dirty_ = true; 
	}

	void RSTextWidget::Resize()
	{
		dirty_ = true;
	}

}
