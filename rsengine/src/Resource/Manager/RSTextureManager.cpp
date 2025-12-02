#include "pch.h"
#include "RSTextureManager.h"
#include <image/stb_image.h>

#define HANGUL_TEXT false

namespace RS_Texture
{
	RSTextureManager::RSTextureManager()
	{
		InitializeText();
	}
	RSTextureManager::~RSTextureManager() = default;


	void RSTextureManager::Initialize()
	{
	}

	void RSTextureManager::InitializeText()
	{
		// All functions return a value different from 0 whenever an error occurred
		if (FT_Init_FreeType(&ft))
		{
			RS_FATAL("ERROR::FREETYPE: Could not init FreeType Library");
			return;
		}

		// find path to font

		const std::string font_name = "C:\\Windows\\Fonts\\HMFMPYUN.ttf";

		if (font_name.empty())
		{
			RS_FATAL("ERROR::FREETYPE: Failed to load font_name");
			return;
		}

		// load font as face
		FT_Face face;
		if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
			RS_FATAL("ERROR::FREETYPE: Failed to load font");
			return;
		}
		else {
			// set size to load glyphs as
			FT_Set_Pixel_Sizes(face, 0, 64);


			// disable byte-alignment restriction
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			// load first 128 characters of ASCII set
			for (unsigned char c = 0; c < 128; c++)
			{
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
				{
					RS_FATAL("ERROR::FREETYTPE: Failed to load Glyph");
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// set texture options
				//glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				RSCharacter character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				m_characters.insert(std::pair<unsigned int, RSCharacter>(c, character));
			}

#if HANGUL_TEXT // Hangul syllables
			for (unsigned int c = 0xAC00; c <= 0xD7A3; c++) { // Hangul syllables
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
				{
					RS_WARN("ERROR::FREETYTPE: Failed to load Glyph");
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// set texture options
				//glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				RSCharacter character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				m_characters.insert(std::pair<unsigned int, RSCharacter>(c, character));
			}
#endif
#if HANGUL_TEXT // Hangul syllables
			for (unsigned int c = 0x3131; c <= 0x3186; c++) { // Hangul syllables
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
				{
					RS_WARN("ERROR::FREETYTPE: Failed to load Glyph");
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// set texture options
				//glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				RSCharacter character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				m_characters.insert(std::pair<unsigned int, RSCharacter>(c, character));
			}
#endif
#if 0 
			for (unsigned int c = 0x2000; c <= 0x2FFF; c++) { // Emozi syllables
				// Load character glyph 
				if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
				{
					RS_WARN("ERROR::FREETYTPE: Failed to load Glyph");
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
				// set texture options
				//glGenerateMipmap(GL_TEXTURE_2D);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				SPH_MINT::RSCharacter character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				m_characters.insert(std::pair<unsigned int, RSCharacter>(c, character));
			}
#endif
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		// destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);


		// configure VAO/VBO for texture quads
		// -----------------------------------
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

	void RSTextureManager::Shutdown()
	{
		// texture deallocate
		for (auto& texture : m_textures)
		{
			texture.second->ReleaseTexture();
			delete texture.second;
		}
		m_textures.clear();
	}

	void RSTextureManager::DrawString(std::wstring text_, glm::vec3 pos, float scale)
	{
		for (std::wstring::const_iterator c = text_.begin(); c != text_.end(); ++c)
		{
			auto [texture_id, size, bearing, advance] = m_characters[*c];

			const float x_pos = pos.x + bearing.x * scale;
			const float y_pos = pos.y - (size.y - bearing.y) * scale;

			const float w = size.x * scale;
			const float h = size.y * scale;
			// update VBO for each character
			const float vertices[6][4] = {
				{ x_pos,     y_pos + h,   0.0f, 0.0f },
				{ x_pos,     y_pos,       0.0f, 1.0f },
				{ x_pos + w, y_pos,       1.0f, 1.0f },

				{ x_pos,     y_pos + h,   0.0f, 0.0f },
				{ x_pos + w, y_pos,       1.0f, 1.0f },
				{ x_pos + w, y_pos + h,   1.0f, 0.0f }
			};
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, texture_id);
			// update content of VBO
			glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);

			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// For next character
			pos.x += (advance >> 6) * scale;
		}
	}

	void RSTextureManager::BindTexture(const std::string& name)
	{
		if (const auto it = m_textures.find(name); it != m_textures.end())
			it->second->BindTexture();
		else
		{
			RS_WARN("Cant find the texture by the name : %s", name.c_str());
		}
	}

	bool RSTextureManager::LoadTextures(const std::string& name, const std::string& path)
	{
		if (const auto texture = new RSTexture(); texture->LoadTexture(path))
		{
			m_textures[name] = texture;
			return true;
		}
		else
		{
			RS_WARN("failed to load texture name : %s", name.c_str());
			delete texture;
			return false;
		}

	}

	bool RSTextureManager::IsExistTexture(const std::string& name) const
	{
		if (m_textures.contains(name))
			return true;

		return false;
	}

	RSTexture* RSTextureManager::GetTexture(const std::string& name)
	{
		if (m_textures.contains(name))
			return m_textures[name];

		RS_WARN("Cant find the texture by the name : %s", name.c_str());
		return GetDefaultTexture();
	}

	unsigned int RSTextureManager::GetTexture_UINT(const std::string& name)
	{
		if (m_textures.contains(name))
			return m_textures[name]->GetTexture();

		RS_WARN("Cant find the texture by the name : %s", name.c_str());
		return GetDefaultTexture_UINT();
	}

	unsigned int RSTextureManager::GetNormalTexture_UINT(const std::string& name)
	{
		if (m_textures.contains(name))
			return m_textures[name]->GetTexture();

		RS_WARN("Cant find the texture by the name : %s", name.c_str());
		return GetDefaultNormalTexture_UINT();
	}

	std::string RSTextureManager::GetTextureName(const unsigned int texture_id) const
  {
    for (const auto& texture : m_textures)
    {
      if (texture.second->GetTexture() == texture_id)
        return texture.first;
    }
    RS_WARN("Cant find the texture by the texture id : %d", texture_id);
		return std::string("NONE");
	}

	void RSTextureManager::CreateTexture(const std::string& name, const int width, const int height)
	{
		const auto texture = new RSTexture();

		texture->CreateTexture(width, height);
		m_textures[name] = texture;
	}

	void RSTextureManager::UpdateTexture(const std::string& name, const unsigned int texture_)
	{
		if (const auto it = m_textures.find(name); it != m_textures.end())
			it->second->SetTexture(texture_);
		else
			RS_WARN("Cant find the texture by the name\n");
	}

	bool RSTextureManager::CheckTexture(const std::string& name)
	{
		if (const auto it = m_textures.find(name); it == m_textures.end())
			return false;

		return true;
	}

	void RSTextureManager::ReleaseTexture(const std::string& name)
	{
		if (const auto it = m_textures.find(name); it != m_textures.end())
		{
			it->second->ReleaseTexture();
			delete it->second;
			m_textures.erase(it);
		}
	}

	GLFWimage RSTextureManager::GetGlfwImageFromPath(const std::string& path)
	{
    int width, height, nr_channels;
    GLFWimage image;
    // Initialize the image struct
    image.width = 0;
    image.height = 0;
    image.pixels = nullptr;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
    if (data)
    {
      image.width = width;
      image.height = height;
      image.pixels = data;
    }
    else
    {
      RS_WARN("failed to load texture. path : %s", path.c_str());
    }
		return image;
	}

}
