/******************************************************************************/
/*!
\file   RSTextureManager.h
\author Jinseob Park
\date   2024/08/01

This file contains Texture Manager for Texture Managing.

*/
/******************************************************************************/
#ifndef RS_TEXTURE_MANAGER_H_
#define RS_TEXTURE_MANAGER_H_

#include <freetype/ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include "glm/glm.hpp"
#include "../Object/Component/Texture/RSTexture.h"
#include "../Object/Component/Texture/RSSkybox.h"
#include "../Widget/RSTextWidget.h"

const std::string LEGEND_TEXTURE_NAME = "legend";
const std::string PARTICLE_TEXTURE_NAME = "jordy_alpha1";


/**
 * @brief Texture namespace
 */
namespace RS_Texture
{

/**
 * @brief Character struct for text rendering
 * It hide struct for users
 */
struct RSCharacter {
	unsigned int texture_id;
	glm::ivec2   size;
	glm::ivec2   bearing;
	unsigned int advance;
};

/**
 * @brief Texture Manager
 */
class RSTextureManager
{
public:
	RSTextureManager();
	~RSTextureManager();

	void Initialize();

	/**
	 * @brief Initialize text rendering
	 * Create text VAO, VBO
	 */
	void InitializeText();

	/**
	 * @brief Release all textures
	 */
	void Shutdown();

	/**
	 * @brief Text render immediately. This is not recommended cuz not efficient.
	 * @param text_ text to render
	 * @param pos position to render on screen space
	 * @param scale text size
	 */
	void DrawString(std::wstring text_, glm::vec3 pos, float scale);

	/**
	 * @brief Bind texture to use
	 * @param name texture name
	 */
	void BindTexture(const std::string& name);

	/**
	 * @brief Load texture from path
	 * @param name texture name 
	 * @param path texture path
	 * @return success or fail
	 */
	bool LoadTextures(const std::string& name, const std::string& path);

	/**
	 * @brief Check texture is exist
	 * @param name texture name to check
	 * @return exist or not
	 */
	bool IsExistTexture(const std::string& name) const;

	/**
	 * @brief Get texture by name
	 * @param name texture name
	 * @return texture pointer. If it is not exist, return default texture
	 */
	RSTexture* GetTexture(const std::string& name);

	/**
	 * @brief Get texture by name
	 * @param name texture name
	 * @return texture pointer. If it is not exist, return default texture UINT
	 */
	unsigned int GetTexture_UINT(const std::string& name);


  /**
   * @brief Get normal texture by name
   * @param name normal texture name
   * @return normal texture pointer. If it is not exist, return default normal texture UINT
   */
  unsigned int GetNormalTexture_UINT(const std::string& name);

	/**
	 * @brief Get text VAO
	 * @return text VAO
	 */
	unsigned int GetTextVAO() const {return text_vao_; }

	/**
	 * @brief Get FT library. FT library is 'freetype' library for text rendering
	 * @return FT library pointer
	 */
	FT_Library* GetFT() { return &ft; }

	/**
	 * @brief Get default texture
	 * @return default texture pointer
	 */
	RSTexture* GetDefaultTexture() { return m_textures["default"]; }

	/**
	 * @brief Get default texture
	 * @return default texture UINT
	 */
	unsigned int GetDefaultTexture_UINT() { return m_textures["default"]->GetTexture(); }

  /**
   * @brief Get default normal texture
   * @return default normal texture pointer
   */
  unsigned int GetDefaultNormalTexture_UINT() { return m_textures["default_normal_map"]->GetTexture(); }

  /**
   * @brief Get texture name by texture id
   * @param texture_id texture id
   * @return texture name
   */
	std::string GetTextureName(unsigned int texture_id) const;

	/**
	 * @brief Get all textures by map
	 * @return texture map <string, RSTexture*>
	 */
	std::map<std::string, RSTexture*>* GetTextures() { return &m_textures; }

	/**
	 * @brief Get all characters for text rendering
	 * @return character unordered_map <unsigned int, RSCharacter>
	 */
	const std::unordered_map<unsigned int, RSCharacter>* GetCharacters() const { return &m_characters; }

	/**
	 * @brief Get text anti aliasing value
	 * @return text anti aliasing value
	 */
	float GetTextAntiAliasing() const { return text_anti_aliasing; }

	/**
	 * @brief Create an empty RSTexture
	 * @param name name of new texture
	 * @param width width of new texture
	 * @param height height of new texture
	 */
	void CreateTexture(const std::string& name, int width, int height);

	/**
	 * @brief Change texture from uint texture id
	 * @param name texture name to change
	 * @param texture_ change texture id	
	 */
	void UpdateTexture(const std::string& name, unsigned int texture_);

	/**
	 * @brief Check is texture exist
	 * @param name texture name to check
	 * @return exist or not
	 */
	bool CheckTexture(const std::string& name);

	/**
	 * @brief Release texture from memory
	 * @param name texture name to release
	 */
	void ReleaseTexture(const std::string& name);


  /**
   * @brief Get png image from path and return GLFW image
   * @param path path to get png image
   * @return GLFWimage
   */
  GLFWimage GetGlfwImageFromPath(const std::string& path);

private:
  std::map<std::string, RSTexture*> m_textures; ///< All textures


  FT_Library ft = nullptr; ///< FreeType library
  std::unordered_map<unsigned int, RSCharacter> m_characters; ///< Characters for text rendering
  unsigned int text_vao_ = 0, text_vbo_ = 0; ///< Text VAO, VBO

  float text_anti_aliasing = 2.0f; ///< Text anti aliasing value

};
}

#endif // !RS_TEXTURE_MANAGER_H_