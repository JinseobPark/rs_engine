/******************************************************************************/
/*!
\file   RSResourceManager.h
\author Jinseob Park
\date   2024/08/01

This file contains Resource Manager for Resource Managing.

*/
/******************************************************************************/
#ifndef RS_RESOURCE_MANAGER_H_
#define RS_RESOURCE_MANAGER_H_

#include "../RSGraphicsData.h"
#include "RSTextureManager.h"
#include "RSShaderManager.h"
#include "RSObjectManager.h"
#include "RSMeshManager.h"
#include "RSBufferManager.h"
#include "RSWidgetManager.h"
#include "RSParticleManager.h"
#include "RSMaterialManager.h"
#include "RSFileIOManager.h"
#include "../Object/Camera/RSCamera.h"

/**
 * @brief Resource Manager.
 * This class is a singleton class.
 * This class manages all the resources in the game engine.
 */
class RSResourceManager
{
public:
	/**
	 * @brief Get the instance of the resource manager.
	 * @return RSResourceManager* instance of the resource manager
	 */
	static RSResourceManager* GetInstance();

	/**
	 * @brief Release the instance of the resource manager.
	 * @return void
	 */
	static void Release();

	/**
	 * @brief Load all the resources.
	 * @see mFileIOManager::Load()
	 * @return void
	 */
	void Load() const;

	/**
	 * @brief Initialize all the resources.
	 * @see mShaderManager::Initialize()
	 * @see mTextureManager::Initialize()
	 * @see mObjectManager::Initialize()
	 * @see mMeshManager::Initialize()
	 * @see mBufferManager::Initialize()
	 * @see mWidgetManager::Initialize()
	 * @see mParticleManager::Initialize()
	 * @see mFileIOManager::Initialize()
	 * @see mCamera::Initialize()
   * @see mMaterialManager::Initialize()
	 * @return void
	 */
	void Initialize() const;

	/**
	 * @brief Post initialize all the resources.
	 */
	void PostInitialize() const;

	/**
	 * @brief Update all the resources.
	 * @param dt delta time
	 * @return void
	 * @see mObjectManager::Update()
	 * @see mBufferManager::Update()
	 * @see mWidgetManager::Update()
	 * @see mParticleManager::Update()
	 * @see mFileIOManager::Update()
	 */
	void Update(float dt) const;

	/**
	 * @brief Shutdown all the resources.
	 * @return void
	 */
	void Shutdown() const;

	/**
	 * @brief Send signal resize to resources the window was resized
	 * @param[in] width_ window width
	 * @param[in] height_ window height
	 * @return void
	 * @see mCamera::UpdateAspect()
	 * @see mBufferManager::ResizeBuffers()
	 * @see mWidgetManager::ResizeWidgets()
	 * @see mParticleManager()
	 */
	void Resize(int width_, int height_) const;

	/**
	 * @brief Get graphics data
	 * @return RSGraphicsData* graphics data
	 */
	RS_N_D RSGraphicsData* GetGraphicsData() const { return m_graphics_data; }

	/**
	 * @brief Get Texture Manager
	 * @return RSTextureManager* texture manager
	 */
	RS_N_D RS_Shader::RSShaderManager* GetShaderManager() const { return m_shader_manager; }

	/**
	 * @brief Get Object Manager
	 * @return RSObjectManager* object manager
	 */
	RS_N_D RS_Texture::RSTextureManager* GetTextureManager() const { return m_texture_manager; }

	/**
	 * @brief Get Mesh Manager
	 * @return RSMeshManager* mesh manager
	 */
	RS_N_D RS_Object::RSObjectManager* GetObjectManager() const { return m_object_manager; }

	/**
	 * @brief Get Mesh Manager
	 * @return RSMeshManager* mesh manager
	 */
	RS_N_D RS_Mesh::RSMeshManager* GetMeshManager() const { return m_mesh_manager; }

	/**
	 * @brief Get Buffer Manager
	 * @return RSBufferManager* buffer manager
	 */
	RS_N_D _RS_Internal::RSBufferManager* GetBufferManager() const { return m_buffer_manager; }

	/**
	 * @brief Get Widget Manager
	 * @return RSWidgetManager* widget manager
	 */
	RS_N_D RS_Widget::RSWidgetManager* GetWidgetManager() const { return m_widget_manager; }

	/**
	 * @brief Get Particle Manager
	 * @return RSParticleManager* particle manager
	 */
	RS_N_D RS_Particle::RSParticleManager* GetParticleManager() const { return m_particle_manager; }

	/**
	 * @brief Get Camera
	 * @return RSCamera* camera
	 */
	RS_N_D RS_Camera::RSCamera* GetCamera() const { return m_camera; }
	
	/**
	 * @brief Get File IO Manager
	 * @return RSFileIOManger* file io manager
	 */
	RS_N_D RS_FILE_IO::RSFileIOManger* GetFileIOManager() const { return m_file_io_manager; }


  /**
   * @brief Get Material Manager
   * @return RSMaterialManager* material manager
   */
  RS_N_D RS_Material::RSMaterialManager* GetMaterialManager() const { return m_material_manager; }


  /**
   * @brief Reset resource level. It just resets graphics data.
	 */
	void ResetResourceLevel();


	RSResourceManager(const RSResourceManager&) = delete;
	RSResourceManager& operator=(const RSResourceManager&) = delete;
private:
	RSResourceManager(); 
	~RSResourceManager();
  static RSResourceManager* m_instance; ///< instance of the resource manager

  RSGraphicsData* m_graphics_data = nullptr; ///< graphics data
  RS_Shader::RSShaderManager* m_shader_manager = nullptr; ///< shader manager
  RS_Texture::RSTextureManager* m_texture_manager = nullptr; ///< texture manager
  RS_Object::RSObjectManager* m_object_manager = nullptr; ///< object manager
  RS_Mesh::RSMeshManager* m_mesh_manager = nullptr; ///< mesh manager
  _RS_Internal::RSBufferManager* m_buffer_manager = nullptr; ///< buffer manager
  RS_Widget::RSWidgetManager* m_widget_manager = nullptr; ///< widget manager
  RS_Particle::RSParticleManager* m_particle_manager = nullptr; ///< particle manager
  RS_Camera::RSCamera* m_camera = nullptr; ///< camera
  RS_Material::RSMaterialManager* m_material_manager = nullptr; ///< material manager

  RS_FILE_IO::RSFileIOManger* m_file_io_manager = nullptr; ///< file io manager
};

#endif // !RS_RESOURCE_MANAGER_H_