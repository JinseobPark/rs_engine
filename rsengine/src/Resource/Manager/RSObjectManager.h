/******************************************************************************/
/*!
\file   RSObjectManager.h
\author Jinseob Park
\date   2024/08/01

This file contains Object Manager

*/
/******************************************************************************/
#ifndef RS_OBJECT_MANAGER_H_
#define RS_OBJECT_MANAGER_H_
#include "../Object/RSObject.h"

/**
* @brief Object namespace
*/
namespace RS_Object
{
using RS_PipelineList::RSRenderLayer;
using RS_PipelineList::RSPipelineType;
using namespace RS_Component;

/**
* @brief Object managing class
*/
class RSObjectManager
{
public :
	RSObjectManager() : m_object_layers(static_cast<size_t>(RSRenderLayer::R_COUNT)), m_target_size_data()
	{
	}

	~RSObjectManager() = default;

	void Initialize();
	/**
	* @brief update all objects
	* @param[in] dt delta time
	*/
	void Update(float dt);
	/**
	* @brief Draw the object that matches the layer
	* Not used.
	* @param type_ Shader type
	*/
	void Draw(RSShaderNames type_) const;

	/**
	* @brief Draw the object model that matches the layer
	* @param[in] type_ object layer type
	* @param[in] shader_ draw shader type
	* @param[in] is_opaque is opaque
	* @return void
	*/
	void DrawModel(RSRenderLayer type_, RSShaderNames shader_, bool is_opaque = true) const;

	/**
	* @brief Draw the object wireframe model that matches the layer
	* @param type_ object layer type
	* @param shader_ draw shader type
	* @return void
	*/
	void DrawWireFrameModel(RSRenderLayer type_, RSShaderNames shader_) const;

	/**
	* @brief Draw the highlight object model that matches the layer
	* @param type_ object layer type
	* @return void
	*/
	void DrawHighlightModel(RSRenderLayer type_) const;


  /**
   * @brief Draw the PBR object model that matches the layer
   * @param type_ object layer type
   * @param shader_ draw shader type
   * @param is_opaque is opaque
   */
  void DrawPbrModel(RSRenderLayer type_, RSShaderNames shader_, const bool is_opaque) const;

	/**
	* @brief Shot down the object manager
	*/
	void Shutdown();

	/**
	* @brief Add object to object manager
	* @param[in] p_object object pointer
	* @return success or fail
	*/
	bool AddObject(RSObject* p_object);

	/**
	* @brief Create object
	* @param[in] name_ object name
	* @return success or fail
	*/
	RSObject* CreateObject(const std::string& name_);

	/**
	* @brief Remove object from object manager
	* @param[in] p_object object pointer
	* @return success or fail
	*/
	bool RemoveObject(RSObject* p_object);


  /**
   * @brief Remove object from object manager
   * @param name_ object name
   * @return success or fail
   */
  bool RemoveObject(const std::string& name_);

	/**
	* @brief Get object from object manager with object name
	* @param name_ object name to search
	* @return object pointer. if not found, return nullptr
	*/
	RSObject* GetObject(const std::string& name_);

	/**
	* @brief Find object from object manager with object name
  *	This function is different with GetObject function at the point that the object must not exist.
	* @param name_ object name to search
	* @return object pointer. if not found, return nullptr
	*/
	RSObject* FindObject(const std::string& name_);


	/**
	* @brief Get boolean value if any object is picked by mouse picking
	* @param[in] x_ x position on the screen to pick object
	* @param[in] y_ y position on the screen to pick object
	* @param[in] is_ctrl To pick multiple objects
	* @return any object is picked or not
	*/
	bool SelectObjectByMousePicking(int x_, int y_, bool is_ctrl);

	/**
		* @brief Unbind all object from the picking list
		*/
	void ReleasePicking() const;


  /**
   * @brief Clear picking object list
	 */
	void ClearPickingObjectList();


  /**
	 * @brief Set picking object.
	 * This function includes 
	 * * Push back to last picking object. 
   * * Push back to highlight object list.
   * * Set last picking object to current picking object.
   * 
	 * @param p_object 
	 */
	void SetPickingObject(RSObject* p_object);

	/**
	* @brief Get object map include all objects
	* @return object map (unordered map)
	*/
	std::unordered_map<std::string, RSObject*>* GetObjects() { return &m_all_objects; }

	/**
	* @brief Get objects list with the layer type
	* @param[in] layer_ layer type
	* @return the layer objects list
	*/
	std::list<RSObject*>& GetObjectLayers(RSRenderLayer layer_) { return m_object_layers[static_cast<size_t>(layer_)]; }

	/**
	* @brief Get objects list all of picked objects
	* @return picked object list
	*/
	std::list<RSObject*>& GetPickedObjectList() { return m_picked_object_list; }

	/**
	* @brief Calculate camera target size
	*/
	void CalculateTargetSizeData();

	/**
	* @brief Get camera target size data
	* @return camera target size data (array of 6 float)
	*/
	std::array<float, 6>& GetTargetSizeData();

	/**
	 * @brief Remove selected object by key input
	 */
	void CalledRemoveObject();


	/**
	 * @brief Create Cube Object
   * @param name_ object name
   * @return The created object
	 */
	RSObject* CreateCubeObject(const std::string& name_ = "");


  /**
   * @brief Create Sphere Object
   * @param name_ object name
	 * @return The created object
	 */
	RSObject* CreateSphereObject(const std::string& name_ = "");


  /**
   * @brief Create Plane Object
   * @param name_ object name
	 * @return The created object
	 */
	RSObject* CreatePlaneObject(const std::string& name_ = "");

	/**
	 * @brief Get collision objects for SPH simulation collision
	 * @return collision objects list
	 */
	std::list<RSObject*>* GetCollisionObjects() { return &m_collision_objects; }


  /**
   * @brief Remove instance objects when level changed.
	 */
	void RemoveObjectsLevelChanged();


  /**
   * @brief Set copy the names of the selected objects to the list.
   * @details copy the names of the selected objects to m_copy_objects_to_name.
   * @param copy_objects_ list of the selected objects
	 */
	void SetCopyObjectsToName(const std::list<RSObject*>& copy_objects_);


  /**
   * @brief Paste the copied objects and set picked object the pasted objects.
   * @details The pasting objects should be copied the object class by 'deep copy'
	 */
	void PasteTheCopiedObjects();


  /**
   * @brief Copy the object with deep copy and return the copied object.
   * @details The copied object's name is set to the name of the original object + "_" + copy number.
   * @param p_object object to copy
   * @return copied object
   */
  RSObject* CopyObject(const RSObject* p_object);


  /**
   * @brief Duplicate the objects with deep copy and return the duplicated objects.
   * @details The duplicated objects' name is set to the name of the original object + "_" + copy number.
   */
	void DuplicateObjects();


  /**
   * @brief Select all objects and highlight in the object manager.
	 */
	void SelectAllObjects();

private:
  std::unordered_map<std::string, RSObject*> m_all_objects; ///< All objects

  std::vector<std::list< RSObject*>> m_object_layers; ///< Object layers
		 
  std::array<float, 6> m_target_size_data; ///< Camera target size data

  std::list<RSObject*> m_picked_object_list; ///< Picked object list

  RSObject* m_last_picked_object = nullptr; ///< Last picked object

  std::list<RSObject*> m_collision_objects; ///< Collision objects for SPH simulation

	// This objects should be removed when level changed.
	std::list<RSObject*> m_instance_objects;

  std::list<std::string> m_copy_objects_to_name; ///< Copy objects to name list

		

};

}

#endif // !RS_OBJECT_MANAGER_H_