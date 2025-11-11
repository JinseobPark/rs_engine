#include "pch.h"
#include "RSLegendWidget.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/Input/RSInputClass.h"
#include <stdlib.h>

namespace RS_Widget
{
	using RS_Input::mRSINPUT;
	
	RSLegendWidget::RSLegendWidget() = default;

	RSLegendWidget::~RSLegendWidget() = default;

	void RSLegendWidget::Initialize() // override
	{
		RSWidgetBase::SetTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("legend")->GetTexture());
		SetOriginTexture(m_texture_id);

		// Get Legend RSTexture from texture manager
		SetTextureSize();

		SetCallback([this] { SetMovable(); });
	}
	void RSLegendWidget::PostInitialize()
	{
	}
	void RSLegendWidget::Update(float dt)
	{
		MoveLegendWidget();
	}
	void RSLegendWidget::DrawVisible()
	{
		RSButtonWidget::DrawVisible();
	}
	void RSLegendWidget::DrawHittable()
	{
		RSButtonWidget::DrawHittable();
	}
	void RSLegendWidget::Destroy()
	{
	}


	void RSLegendWidget::CreateDividedTexture(const unsigned int divided_value_)
	{
		RSResourceManager* resource_manager = RSResourceManager::GetInstance();
		// if "legend_divided" texture is already loaded, release it.
		resource_manager->GetTextureManager()->ReleaseTexture("legend_divided");

		// Get Legend RSTexture from texture manager
		const unsigned int legend_texture = m_origin_texture;

		const auto legend_texture_data = new unsigned char[m_legend_width * m_legend_height * 4];
    glGetTextureImage(legend_texture, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_legend_width * m_legend_height * 4,
                      legend_texture_data);

		// print legend_texture_data
		//for (unsigned int i = 0; i < get_textured_width * get_textured_height * 4; i += 4)
		//{
		//	std::cout << "r: " << (int)legend_texture_data[i] << " g: " << (int)legend_texture_data[i + 1] << " b: " << (int)legend_texture_data[i + 2] << " a: " << (int)legend_texture_data[i + 3] << std::endl;
		//}

		// Create new texture to empty image by vertical image
		resource_manager->GetTextureManager()->CreateTexture("legend_divided", m_legend_width, m_legend_height);

		// Get divided texture from texture manager
		m_divided_texture = resource_manager->GetTextureManager()->GetTexture("legend_divided")->GetTexture();

		if (divided_value_ == 0)
		{
			// data copy from legend_texture to divided texture
			glBindTexture(GL_TEXTURE_2D, m_divided_texture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_legend_width, m_legend_height, GL_RGBA, GL_UNSIGNED_BYTE,
                      legend_texture_data);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			// Create a frame buffer
			GLuint temp_frame_buffer;
			glGenFramebuffers(1, &temp_frame_buffer);
			glBindFramebuffer(GL_FRAMEBUFFER, temp_frame_buffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_divided_texture, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				RS_WARN("Framebuffer not complete!");
				return;
			}

			// Create VAO, VBO to draw 2d box
			unsigned int temp_vao, temp_vbo;
			glGenVertexArrays(1, &temp_vao);
			glGenBuffers(1, &temp_vbo);
			glBindVertexArray(temp_vao);
		
			// First of all, bind VBO to draw 2d box, fill it with 0
			glBindBuffer(GL_ARRAY_BUFFER, temp_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, NULL, GL_DYNAMIC_DRAW); // 6 vertices, 2 floats per vertex
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			// Set viewport to divided texture size
			glViewport(0, 0, m_legend_width, m_legend_height);
			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClear(GL_COLOR_BUFFER_BIT);

			// Set shader to divide texture shader
			resource_manager->GetShaderManager()->Use(RS_Shader::RSShaderNames::SOLID_COLOR);
			resource_manager->GetShaderManager()->SetData(RS_Shader::RSShaderNames::SOLID_COLOR, "projection",
                                                    glm::ortho(0.0f, static_cast<float>(m_legend_width), 0.0f,
                                                               static_cast<float>(m_legend_height)));


			// calculate divided height size
			const float divided_height = static_cast<float>(m_legend_height) / static_cast<float>(divided_value_);

			float x = 0.0f;
			float y = 0.0f;

			glBindVertexArray(temp_vao);
			for (unsigned int i = 0; i < divided_value_; i++)
			{
				// Set color size to draw 2d box
				const auto start_pos = glm::vec2(x, y);
				const auto end_pos = glm::vec2(m_legend_width, y + divided_height);
				const auto mid_pos = glm::vec2(static_cast<float>(m_legend_width) / 2.0f, y + divided_height / 2.0f);

				// Spoil color from legend texture
				const auto mid_pos_in_texture = glm::vec2(mid_pos.x / static_cast<float>(m_legend_width),
                                                  mid_pos.y / static_cast<float>(m_legend_height));

				// get_textured_width, get_textured_height
				const unsigned char r = legend_texture_data[
          static_cast<int>(mid_pos_in_texture.y * m_legend_height * m_legend_width + mid_pos_in_texture.x *
                           m_legend_width) * 4];
				const unsigned char g = legend_texture_data[
          static_cast<int>(mid_pos_in_texture.y * m_legend_height * m_legend_width + mid_pos_in_texture.x *
                           m_legend_width) * 4 + 1];
				const unsigned char b = legend_texture_data[
          static_cast<int>(mid_pos_in_texture.y * m_legend_height * m_legend_width + mid_pos_in_texture.x *
                           m_legend_width) * 4 + 2];
				const unsigned char a = legend_texture_data[
          static_cast<int>(mid_pos_in_texture.y * m_legend_height * m_legend_width + mid_pos_in_texture.x *
                           m_legend_width) * 4 + 3];

				// Set color to draw 2d box
				const auto color = glm::vec4(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f,
                                     static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f);

				// Set color data to shader
				resource_manager->GetShaderManager()->SetData(RS_Shader::RSShaderNames::SOLID_COLOR, "solid_color", color);

				// Draw 2d box
				const float vertices[6][2] = {
					{start_pos.x, start_pos.y},
					{end_pos.x, start_pos.y},
					{end_pos.x, end_pos.y},

					{end_pos.x, end_pos.y},
					{start_pos.x, end_pos.y},
					{start_pos.x, start_pos.y}
				};

				glBindBuffer(GL_ARRAY_BUFFER, temp_vbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glDrawArrays(GL_TRIANGLES, 0, 6);

				y += divided_height;
			}
			glBindVertexArray(0);


			// Release frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &temp_frame_buffer);

			// Release VAO, VBO
			glDeleteVertexArrays(1, &temp_vao);
			temp_vao = 0;
			glDeleteBuffers(1, &temp_vbo);
			temp_vbo = 0;
		}

		// Check that b_is_vertical is true, divided_texture rotate 90 degree
		SwitchVerticalImage();

		// Set divided texture to widget texture
		RSWidgetBase::SetTexture(m_divided_texture);
		
		// Release legend_texture_data
		delete[] legend_texture_data;

	}
	void RSLegendWidget::SetDividedValue(const unsigned int divided_value_)
	{
		divided_value = divided_value_;
		CreateDividedTexture(divided_value);
	}

	void RSLegendWidget::MoveLegendWidget()
	{
		// calculate mouse position to -1 ~ 1
		if (b_movable)
		{
			const float pos_x = (2.0f * static_cast<float>(mRSINPUT->GetMousePosition().x)) / static_cast<float>(
                            RS_Application::mRSAPPLICATION->GetWidth()) - 1.0f;
			const float pos_y = 1.0f - (2.0f * static_cast<float>(mRSINPUT->GetMousePosition().y)) / static_cast<float>(
                            RS_Application::mRSAPPLICATION->GetHeight());

			SetPosition(pos_x, pos_y);
		}
	}

	void RSLegendWidget::SetVertical(const bool is_vertical_)
	{
		if (b_is_vertical == is_vertical_)
			return;
		b_is_vertical = is_vertical_;
		CreateDividedTexture(divided_value);
		TransposeSize();
	}

	void RSLegendWidget::SwitchVerticalImage() const
	{
		if (!b_is_vertical)
		{

			glBindTexture(GL_TEXTURE_2D, m_divided_texture);
			const auto divided_texture_data = new unsigned char[m_legend_width * m_legend_height * 4];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, divided_texture_data);

			const auto rotated_texture_data = new unsigned char[m_legend_width * m_legend_height * 4];
			for (int i = 0; i < m_legend_width; i++)
			{
				for (int j = 0; j < m_legend_height; j++)
				{
					rotated_texture_data[(int)(i * m_legend_height + j) * 4] = divided_texture_data[(int)(j * m_legend_width + i) * 4];
					rotated_texture_data[(int)(i * m_legend_height + j) * 4 + 1] = divided_texture_data[
            (int)(j * m_legend_width + i) * 4 + 1];
					rotated_texture_data[(int)(i * m_legend_height + j) * 4 + 2] = divided_texture_data[
            (int)(j * m_legend_width + i) * 4 + 2];
					rotated_texture_data[(int)(i * m_legend_height + j) * 4 + 3] = divided_texture_data[
            (int)(j * m_legend_width + i) * 4 + 3];
				}
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_legend_height, m_legend_width, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                   rotated_texture_data);
			glBindTexture(GL_TEXTURE_2D, 0);

			delete[] divided_texture_data;
			delete[] rotated_texture_data;
		}
	}



	void RSLegendWidget::Released()
	{
		b_movable = false;
	}

	void RSLegendWidget::SetMovable()
	{
		b_movable = true;
	}

	void RSLegendWidget::SetTexture(const unsigned int texture_id_)
	{
    m_texture_id = texture_id_;
		SetOriginTexture(m_texture_id);

		// divided value reset
		divided_value = 0;
		SetTextureSize();

    // if the widget is not vertical, create divided texture with the divided value
		if (!b_is_vertical)
			CreateDividedTexture(divided_value);

	}
	void RSLegendWidget::SetTextureSize()
	{
		// Get data the origin legend texture for get color by position using glReadPixels
		glBindTexture(GL_TEXTURE_2D, m_origin_texture);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_legend_width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_legend_height);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}