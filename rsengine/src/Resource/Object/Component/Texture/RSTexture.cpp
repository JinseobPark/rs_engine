#include "pch.h"
#include "RSTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <image/stb_image.h>
#include <filesystem>
#include <iostream>
#include "GL/glew.h"

namespace RS_Texture
{
	void RSTexture::ReleaseTexture()
	{
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}

	void RSTexture::CreateTexture(const int width, const int height)
	{
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void RSTexture::SetTexturePath(const std::string& path, const bool is_exist_)
	{
		texture_path_ = path;
		is_exist_path_ = is_exist_;
	}

	bool RSTexture::LoadTexture(const std::string& path_)
	{
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);

		int width, height, nr_channels;

		if (unsigned char* data = stbi_load(path_.c_str(), &width, &height, &nr_channels, 0))
		{
			GLenum format = GL_RED;
			if (nr_channels == 1)
				format = GL_RED;
			else if (nr_channels == 3)
				format = GL_RGB;
			else if (nr_channels == 4)
				format = GL_RGBA;


			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
			stbi_image_free(data);

			// Update load path
			SetTexturePath(path_, true);
			return true;
		}
		else
		{
			RS_WARN("failed to load texture. path : %s", path_.c_str());
			stbi_image_free(data);
			return false;
		}
	}

	RSTexture::~RSTexture()
	{
		texture_path_.clear();
	}

	void RSTexture::BindTexture() const
	{
		glBindTexture(GL_TEXTURE_2D, texture_);
	}
}
