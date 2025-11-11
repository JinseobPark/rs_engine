/******************************************************************************/
/*!
\file   RSSTLIO.h
\author Jinseob Park
\date   2024/08/01

This file contains loading STL file for Rendering System.

*/
/******************************************************************************/
#ifndef RSSTLIO_H
#define RSSTLIO_H

#include <stdio.h>

/**
 * @brief File IO namespace
 */
namespace RS_FILE_IO
{
/**
* @brief stl file IO class
*/
class RSSTLIO
{
public:
	RSSTLIO() = default;
	~RSSTLIO() = default;

	/**
	 * @brief Import stl file
	 * @param[in] path_ - stl file path (string)
	 * @param[out] data_ - Reference of mesh data for result
	 * @return success or fail
	 */
	static bool ImportStl(const std::string& path_, RSMeshData& data_);

	/**
	 * @brief Import stl file
	 * @param[in] path_ - stl file path (wstring)
	 * @param[out] data_ - Reference of mesh data for result
	 * @return success or fail
	 */
	static bool ImportStl(const std::wstring& path_, RSMeshData& data_);


  /**
	 * @brief 
	 * @param mesh_data static constexpr unsigned char GIZMO_SCALE_CUBE[] 
	 * @param data_ 
   * @return success or fail
	 */
	static bool ImportEmbeddedMeshData(const unsigned char mesh_data[], RSMeshData& data_);

private:
	/**
	* @brief Read binary data
	* @param[in] fp_ - file pointer
	* @param[out] data_ - Reference of mesh data for result
	* @return success or fail
	*/
	static bool ReadDataBinary(FILE* fp_, RSMeshData& data_);

	/**
	 * @brief Read ascii data
	 * @param[in] fp_ - file pointer
	 * @param[out] data_ - Reference of mesh data for result
	 * @return success or fail
	 */
	static bool ReadDataAscii(FILE* fp_, RSMeshData& data_);



	// try Import stl file > check data type > read data > return result
};
}
#endif