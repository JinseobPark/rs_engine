#include "pch.h"
#include "RSResourceManager.h"
#include "Systems/Application/RSApplication.h"


RSResourceManager* RSResourceManager::m_instance = nullptr;

RSResourceManager* RSResourceManager::GetInstance()
{
	if (m_instance == nullptr)
		{
			m_instance = new RSResourceManager();
		}
		return m_instance;
}


void RSResourceManager::ResetResourceLevel()
{
	RS_ASSERT(m_graphics_data != nullptr, "Resource graphics data is nullptr");
	m_graphics_data->Reset();
}

RSResourceManager::RSResourceManager()
{
	m_graphics_data = new RSGraphicsData();
	m_shader_manager = new RS_Shader::RSShaderManager();
	m_texture_manager = new RS_Texture::RSTextureManager();
	m_object_manager = new RS_Object::RSObjectManager();
	m_mesh_manager = new RS_Mesh::RSMeshManager();
	m_buffer_manager = new _RS_Internal::RSBufferManager();
	m_widget_manager = new RS_Widget::RSWidgetManager();
	m_particle_manager = new RS_Particle::RSParticleManager();
	m_camera = new RS_Camera::RSCamera();
	m_file_io_manager = new RS_FILE_IO::RSFileIOManger();
  m_material_manager = new RS_Material::RSMaterialManager();
}

void RSResourceManager::Release()
{

	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

void RSResourceManager::Load() const
{
	m_file_io_manager->Load();
}

void RSResourceManager::Initialize() const
{
	m_shader_manager->Initialize();
	m_shader_manager->Compile();
	m_texture_manager->Initialize();
	m_object_manager->Initialize();
	m_mesh_manager->Initialize();
	m_buffer_manager->Initialize();
	m_widget_manager->Initialize();
	m_particle_manager->Initialize();
	m_file_io_manager->Initialize();
	m_camera->Initialize();

	m_camera->UpdateAspect(static_cast<int>(RS_Application::mRSAPPLICATION->GetWidth()),
                         static_cast<int>(RS_Application::mRSAPPLICATION->GetHeight()));

	m_material_manager->Initialize();

}

void RSResourceManager::PostInitialize() const
{
	//mShaderManager->PostInitialize();
	//mTextureManager->PostInitialize();
	//mObjectManager->PostInitialize();
	//mMeshManager->PostInitialize();
	m_buffer_manager->PostInitialize();
	m_widget_manager->PostInitialize();
	//mParticleManager->PostInitialize();
	//mCamera->PostInitialize();
}

void RSResourceManager::Update(const float dt) const
{
	//mShaderManager->Update();
	//mTextureManager->Update();
	//mMeshManager->Update();
	m_object_manager->Update(dt);
	m_buffer_manager->Update(dt);
	m_widget_manager->Update(dt);
	m_particle_manager->Update(dt);
	m_file_io_manager->Update(dt);
	//mCamera->Update();
}

void RSResourceManager::Shutdown() const
{
	m_shader_manager->Shutdown();
	m_texture_manager->Shutdown();
	m_object_manager->Shutdown();
	m_mesh_manager->Shutdown();
	m_buffer_manager->Shutdown();
	m_widget_manager->Shutdown();
	m_particle_manager->Shutdown();
	m_file_io_manager->Shutdown();
  m_material_manager->Shutdown();

}

void RSResourceManager::Resize(const int width_, const int height_) const
{
	GetCamera()->UpdateAspect(width_, height_);
	GetBufferManager()->ResizeBuffers(width_, height_);
	GetWidgetManager()->ResizeWidgets();
	if(GetParticleManager()->IsExistParticleSimulator())
		GetParticleManager()->GetParticleSimulator()->UpdateRenderProperty();
}

RSResourceManager::~RSResourceManager()
{
	delete m_graphics_data;
	delete m_shader_manager;
	delete m_texture_manager;
	delete m_object_manager;
	delete m_mesh_manager;
	delete m_buffer_manager;
	delete m_widget_manager;
	delete m_particle_manager;
	delete m_camera;
	delete m_file_io_manager;
  delete m_material_manager;
}

