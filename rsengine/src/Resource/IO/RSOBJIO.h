/******************************************************************************/
/*!
\file   RSOBJIO.h
\author Jinseob Park
\date   2024/08/01

This file contains loading STL file for Rendering System.

*/
/******************************************************************************/
#ifndef RS_OBJIO_H_
#define RS_OBJIO_H_


//#include "Define/RSGeometryData.h"
//#include "glm/glm.hpp"

/**
 * @brief File IO namespace
 */
namespace RS_FILE_IO
{
/**
* @brief Obj file IO class
*/
class RSOBJIO
{
public:
	RSOBJIO() = default;
	~RSOBJIO() = default;

	/**
	* @brief Obj file import function. BUT, this function is not used in the project. PLEASE USE ImportOBJ_version2 instead.
	* @param[in] path_ : Obj file path
	* @param[out] data_ : Reference of mesh data
	* @return success or fail
	*/
	static bool ImportObjOld(const std::string& path_, RSMeshData& data_);

	/**
	* @brief Obj file import function
	* @param[in] path_ : Obj file path
	* @param[out] data_ : Reference of mesh data
	* @return success or fail
	*/
	static bool ImportObj(const std::string& path_, RSMeshData& data_);

	/**
	* @brief Obj file import function
	* @param[in] path_ : Obj file path
	* @param[out] data_ : Reference of mesh data
	* @return success or fail
	*/
	static bool ImportObj(std::wstring path_, RSMeshData& data_);

};
}

#endif // !RS_OBJIO_H_
