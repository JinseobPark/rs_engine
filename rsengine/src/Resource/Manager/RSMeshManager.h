/******************************************************************************/
/*!
\file   RSMeshManager.h
\author Jinseob Park
\date   2024/08/01

This file contains 3D Geometry Mesh Manager.

*/
/******************************************************************************/
#ifndef RS_MESH_MANAGER_H_
#define RS_MESH_MANAGER_H_
#include "../Object/Component/Mesh/RSMesh.h"
#include "../Object/Component/Mesh/RSMeshSTL.h"
#include "../Object/Component/Mesh/RSGeometryGenerator.h"
#include <map>

/**
* @brief Mesh namespace
*/
namespace RS_Mesh
{
/**
* @brief Mesh managing class
*/
class RSMeshManager : public RSGeometryGenerator
{
public:
	RSMeshManager();
	~RSMeshManager();

	void Initialize();
	void Shutdown();


	/**
	* @brief Load obj mesh from string path
	* @param[in] name_ Set the mesh name when it successfully loaded.
	* @param[in] path_ File path to load (string)
	* @return success or fail
	*/
	bool LoadMesh(const std::string& name_, std::string path_);
	//bool LoadMeshSTL(std::string name_, std::string path_); // NOT USED

	/**
	* @brief Load stl mesh from path
	* @param[in] name_ Set the mesh name when it successfully loaded.
	* @param[in] path_ File path to load
	* @return success or fail
	*/
	bool LoadMeshStl2(const std::string& name_, std::string path_);
		
	/**
	* @brief Load obj mesh from wstring path
	* @param[in] name_ Set the mesh name when it successfully loaded.
	* @param[in] path_ File path to load (wstring)
	* @return success or fail
	*/
	bool LoadMesh(const std::string& name_, const std::wstring& path_);

	/**
	* @brief Load stl mesh from wstring path
	* @param[in] name_ Set the mesh name when it successfully loaded.
	* @param[in] path_ File path to load (wstring)
	* @return success or fail
	*/
	bool LoadMeshStl2(const std::string& name_, const std::wstring& path_);

	/**
	* @brief Check the mesh exist
	* @param[in] name_ Mesh name to check
	* @return exist or not
	*/
	RS_N_D bool IsExistMesh(const std::string& name_) const;

	/**
	* @brief Get mesh data
	* @param[in] name_ Mesh name to get
	* @return Mesh data
	*/
	RSMeshData* GetMesh(const std::string& name_) { return m_meshes[name_]; }

	/**
	* @brief Get all meshes
	* @return Mesh data
	*/
	std::unordered_map<std::string, RSMeshData*>* GetMeshes() { return &m_meshes; }
	
	/**
	* @brief Release (unload) mesh
	* @param name_ Mesh name to release
	*/
	void ReleaseMesh(const std::string& name_);


  /**
   * @brief Generate initial meshes
   * @details This function generates initial meshes such as Cube, Plane, Grid, Sphere, Sphere2, Cube2
	 */
	void GenerateInitMeshes();

  /**
   * @brief Generate initial gizmo meshes
	 */
	void InitGizmoMeshes();

  /**
    * @brief Generate initial gizmo mesh
    * @param[in] mesh_name Mesh name to generate
    * @param[in] mesh_data Mesh data to generate
    */
	void GenerateInitGizmoMesh(const std::string& mesh_name, const unsigned  char mesh_data[]);

private:
  std::unordered_map<std::string, RSMeshData*> m_meshes; ///< Mesh data map
};
}

#endif // !RS_MESH_MANAGER_H_