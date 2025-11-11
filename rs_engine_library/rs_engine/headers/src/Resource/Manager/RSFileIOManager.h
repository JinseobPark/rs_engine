/******************************************************************************/
/*!
\file	RSFileIOManager.h
\author Jinseob Park
\date   2024/08/01

This file contains File IO System manager
*/
/******************************************************************************/
#ifndef RS_FILEIO_MANAGER_H_
#define RS_FILEIO_MANAGER_H_
#include "tinyfiledialogs/tinyfiledialogs.h"

/**
* @brief File IO namespace
*/
namespace RS_FILE_IO
{
/**
* @brief File IO Manager
*/
class RSFileIOManger
{
public:
	RSFileIOManger();
	~RSFileIOManger();

	void Load();
	void Initialize();

	void Update(float dt);

	void Shutdown();

	/**
	 * @brief When the file dialog is ready, it will try to open the file.
	 * @ When the file dialog already opened, it will do nothing.
	 * @see IsLoaderReady()
	 * @return void
	 */
	void TryFileIO();

	/**
	 * @brief Load all init files. 
	 * The path is resources/InitLoadFiles
	 * @see LoadTextures()
	 * @see LoadMeshes();
	 * @return void
	 */
	void LoadInitFiles();


	/**
	 * @brief Open file with file dialog.
	 *
	 * @return success or fail
	 */
	bool OpenFile();

	/**
	 * @brief Open file with file dialog.
	 * @param[in] path_ file path (string)
	 * @return success or fail
	 */
	static bool OpenFile(std::string path_);

	/**
	 * @brief Open file with file dialog.
	 * @param[in] path_ file path (wstring)
	 * @return success or fail
	 */
	static bool OpenFile(const std::wstring& path_);

	/**
	 * @brief Open file with file dialog.
	 * @param path_ file path (char*)
	 * @return success or fail
	 */
	static bool OpenFile(const char* path_);

	/**
	 * @brief Save File Dialog for file IO.
   * It uses tinyfd library for save file dialog.
	 */
	bool SaveFileDialog(const std::wstring& extension_ = L"");
protected:
	/**
	* @brief Open file dialog for file IO.
	* It uses tinyfd library for open file dialog.
	* But, open file function is use the TryFileIO function.
	* @see TryFileIO
	* @return success or fail
	*/
	bool OpenFileDialog();


	/**
	 * @brief Link managers (Texture, Mesh)
	 */
	void LinkManagers();

	/**
	 * @brief Load init textures in resources/InitLoadFiles/Textures
	 */
	void LoadTextures() const;

	/**
	 * @brief Load init meshes in resources/InitLoadFiles/Meshes
	 */
	void LoadMeshes() const;

	/**
	 * @brief The file dialog is ready or not.
	 * @return true or false
	 */
	RS_N_D bool IsLoaderReady() const { return m_loader_is_ready; }

	/**
	 * @brief Set the file dialog is ready or not.
	 * @param[in] is_ready true or false
	 */
	void SetLoaderReady(const bool is_ready) { m_loader_is_ready = is_ready; }

	/**
	 * @brief Check the file dialog system is ready or not 
	 * @return true or false
	 */
	bool IsFileIODone();

	/**
	 * @brief Get the file IO result from the file dialog system.
	 * @return success or failed
	 */
	bool GetFileIOResult();

	/**
	 * @brief Close the file dialog.
	 */
	void CloseFileDialog();



	/**
	* @brief Get the file path from the file dialog system.
	* @return file path (wstring)
	*/
	std::wstring GetFilePath() { return m_file_path; }

	/**
	 * @brief return false for file IO result
	 * @return false
	 */
	bool ReturnFalseIoResult();
private:
  std::future<bool> m_file_io_result; ///< file IO result

  bool m_loader_is_ready = true; ///< file dialog is ready or not
  std::wstring m_file_path; ///< file path

  RS_Texture::RSTextureManager* m_texture_manager = nullptr; ///< texture manager
  RS_Mesh::RSMeshManager* m_mesh_manager = nullptr; ///< mesh manager

};

}

#endif // !RS_FILEIO_MANAGER_H_