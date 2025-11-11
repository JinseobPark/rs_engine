/******************************************************************************/
/*!
\file   RSTexture.h
\author Jinseob Park
\date   2024/08/01

This file contains texture class for object, 
	which includes texture path, texture id, and texture functions.

*/
/******************************************************************************/
#ifndef RS_TEXTURE_H_
#define RS_TEXTURE_H_

/**
 * @brief Texture namespace
 */
namespace RS_Texture
{
	/**
	 * @brief Texture Class
	 */
	class RSTexture
	{
	public:
		RSTexture() = default;
		~RSTexture();

		/**
		 * @brief Get texture id
		 * @return texture id
		 */
		unsigned int GetTexture() const { return texture_; }
		/**
		 * @brief Set texture by texture id (uint)
		 * @param texture id (uint)
		 */
		void SetTexture(const unsigned int texture) { texture_ = texture; }

		/**
		 * @brief Bind texture to GL_TEXTURE_2D.
		 */
		void BindTexture() const;

		/**
		 * @brief Load texture by path
		 * @param path_ texture path
		 * @return success or fail
		 */
		bool LoadTexture(const std::string& path_);

		/**
		 * @brief Delete texture from memory
		 */
		void ReleaseTexture();

		/**
		 * @brief Create texture by width and height
		 * @param width texture width
		 * @param height texture height
		 */
		void CreateTexture(int width, int height);

		/**
		 * @brief Check if texture path exists
		 * @return exist or not
		 */
		bool IsExistPath() const { return is_exist_path_; }

		/**
		 * @brief Get texture path
		 * @return texture path
		 */
		std::string GetTexturePath() { return texture_path_; }

		/**
		 * @brief Set texture path
		 * @param path texture path
		 * @param is_exist_ exist or not
		 */
		void SetTexturePath(const std::string& path, bool is_exist_ = true);

		/**
		 * @brief Set exist path
		 * @param is_exist exist or not
		 */
		void SetExistPath(const bool is_exist) { is_exist_path_ = is_exist; }

	private:
    unsigned int texture_ = 0; ///< Texture id
    bool is_exist_path_ = false; ///< Is texture path exist
    std::string texture_path_; ///< Texture path
	};
}

#endif // !RS_TEXTURE_H_