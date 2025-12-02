#include "pch.h"
#include "ParticleSimulatorLevel.h"
#include "../LevelData/ParticleSimulatorData.h"
#include "../Widget/ParticleSimulatorWidget.h"

#include "GameData.h"

namespace SJG
{
	ParticleSimulatorData* m_particle_simulator_data = nullptr;
	ParticleSimulatorWidget* m_particle_simulator_widget = nullptr;
	RS_Particle::RSParticleSimulator* m_particle_simulator = nullptr;

	void ParticleSimulatorLoad()
	{
		m_resource_manager = RSResourceManager::GetInstance();
		m_resource_manager->GetParticleManager()->CreateParticleSimulator();
		m_particle_simulator_data = new ParticleSimulatorData();
		m_particle_simulator_widget = new ParticleSimulatorWidget();
		m_particle_simulator = m_resource_manager->GetParticleManager()->GetParticleSimulator();

		// Initialize Particle System
		RSInitSettingParticleSystem particle_init_setting;
		particle_init_setting.boundary_min = glm::vec3(-15, -1, -5);
		particle_init_setting.boundary_max = glm::vec3(15, 10, 5);
		particle_init_setting.init_particle_setting_min = glm::vec3(-14.9f, 0.1f, -4.9f);
		particle_init_setting.init_particle_setting_max = glm::vec3(-7.0f, 9.5f, 4.9f);
		particle_init_setting.init_particle_setting_space = 0.25f;
		//particle_init_setting.init_grid_resolution = glm::vec3(5, 5, 5);
		particle_init_setting.init_grid_resolution = (particle_init_setting.boundary_max - particle_init_setting.boundary_min) * 0.5f;
		particle_init_setting.grid_max_count_per_max = 0.01f; // 2 percent 

		particle_init_setting.m_inflow_max_count = 50000;
    particle_init_setting.m_inflow_direction = glm::vec3(1, 0, 0);
    particle_init_setting.m_inflow_speed = 10.0f;
    particle_init_setting.m_inflow_position = glm::vec3(-5, 5, 0);
    particle_init_setting.m_inflow_radius = 2.0f;
		particle_init_setting.m_inflow_zone_size = glm::vec3(particle_init_setting.m_inflow_radius);
		particle_init_setting.m_outflow_position = glm::vec3(5, 2, 0);
		particle_init_setting.m_outflow_zone_size = glm::vec3(1, 10, 10);



		m_particle_simulator_data->SetInitSettingParticleSystem(particle_init_setting);
		RSParticleProperty particle_property;
		particle_property.particle_radius = particle_init_setting.init_particle_setting_space * 0.5f;
		particle_property.smoothing_length = particle_init_setting.init_particle_setting_space * 2.0f;
		particle_property.collision_stiffness = 100.0f;
		particle_property.density = 1000.0f;
		particle_property.bulk = 1000.0f;
		particle_property.viscosity = 200.0f;
		particle_property.gravity = 9.8f;
		particle_property.mass = 1.0f; // Should be calculated by density and volume                                                                                             
		particle_property.gas_constant = 2000.0f;

		m_particle_simulator_data->SetParticleProperty(particle_property);

		m_particle_simulator->InitSetSolverType(RSSolverType::R_STATIC_BRUTE_FORCE);
		//{ // Particle boundary Box
		//	auto boundary_box = m_resource_manager->GetObjectManager()->CreateObject("PS_boundary_box");

		//	auto boundary_box_model = new RS_Component::RSModel();
		//	boundary_box->AddComponent(boundary_box_model);

		//	auto cube_mesh = new RS_Mesh::RSMesh();
		//	cube_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Cube"));
		//	cube_mesh->SetIsTexture(true);

		//	boundary_box_model->SetMesh(cube_mesh);
		//	boundary_box->SetRenderLayer(RS_PipelineList::RSRenderLayer::R_LINE_OPAQUE);

		//	glm::vec3 boundary_box_position = particle_init_setting.boundary_min + (particle_init_setting.boundary_max - particle_init_setting.boundary_min) * 0.5f;
		//	glm::vec3 boundary_box_scale = particle_init_setting.boundary_max - particle_init_setting.boundary_min;
		//	boundary_box->GetTransform()->SetPosition(boundary_box_position);
		//	boundary_box->GetTransform()->SetScale(boundary_box_scale);
		//}

		//{ // Particle init setting box
		//	auto setting_box = m_resource_manager->GetObjectManager()->CreateObject("PS_setting_box");

		//	auto setting_box_model = new RS_Component::RSModel();
		//	setting_box->AddComponent(setting_box_model);

		//	auto cube_mesh = new RS_Mesh::RSMesh();
		//	cube_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Cube"));
		//	cube_mesh->SetIsTexture(true);

		//	setting_box_model->SetMesh(cube_mesh);
		//	setting_box->SetRenderLayer(RS_PipelineList::RSRenderLayer::R_LINE_OPAQUE);

		//	glm::vec3 setting_box_position = particle_init_setting.init_particle_setting_min + (particle_init_setting.init_particle_setting_max - particle_init_setting.init_particle_setting_min) * 0.5f;
		//	glm::vec3 setting_box_scale = particle_init_setting.init_particle_setting_max - particle_init_setting.init_particle_setting_min;
		//	setting_box->GetTransform()->SetPosition(setting_box_position);
		//	setting_box->GetTransform()->SetScale(setting_box_scale);
		//}

		RS_Application::mRSAPPLICATION->SetWindowIcon("resources/InitLoadFiles/Textures/bono.png");
		RS_Application::mRSAPPLICATION->SetWindowTitle("RS Engine Fluid Simulator");

		RS_MSG("Particle Simulator Level Loaded");
	}

	void ParticleSimulatorInitialize()
	{
		m_particle_simulator_data->Initialize();
		m_resource_manager->GetParticleManager()->SetUseSph(true);




		//auto init_setting = m_particle_simulator_data->GetInitSettingParticleSystem();
		//init_setting.boundary_max = boundary_box_max;
		//init_setting.boundary_min = boundary_box_min;
		//init_setting.init_particle_setting_min = setting_box_min;
		//init_setting.init_particle_setting_max = setting_box_max;


		m_particle_simulator->SetInitSettingParticleSystem(m_particle_simulator_data->GetInitSettingParticleSystem());

		m_particle_simulator->SetParticleProperty(m_particle_simulator_data->GetParticleProperty());

		// Get collision objects from object manager.
		auto collision_objects = m_resource_manager->GetObjectManager()->GetCollisionObjects();
		for (const auto collision_object : *collision_objects)
		{
			m_particle_simulator->AddMeshDataToCollision(collision_object);
		}

		m_particle_simulator->Initialize();
		m_particle_simulator->SetUsingTexture(true);
		m_particle_simulator->SetPlaying(false);
		
		m_particle_simulator_widget->Initialize();

	}

	void ParticleSimulatorUpdate(float dt)
	{
		m_particle_simulator_data->Update(dt);
		m_particle_simulator_widget->Update(dt);

		//m_resource_manager->GetParticleManager()->GetParticleSimulator()->Update(dt);

		GameData::GetInstance()->Update(dt);

		//// sine float number by total time.
  //  float time = 10.0f * glm::mod(RS_Timer::mRSTIMER->GetTotalTime(), 360.0f);

		//// Get object name inflow sphere
  // // auto inflow_sphere = m_resource_manager->GetObjectManager()->GetObject(INFLOW_SPHERE_OBJECT_NAME);
  // // if (inflow_sphere != nullptr)
		//	//inflow_sphere->GetTransform()->SetPosition(glm::vec3(-5.0f * cos(time), 5.0f, 5.0f * sin(time)));

		//// Direction rotate by timer
  //  glm::vec3 cur_dir = m_particle_simulator->GetInitSettingParticleSystem().m_inflow_direction;
  //  m_particle_simulator->SetInflowDirection(glm::vec3(sin(time), cur_dir.y, -cos(time)));


	}

	void ParticleSimulatorShutdown()
	{
		m_resource_manager->GetParticleManager()->GetParticleSimulator()->Shutdown();
		m_resource_manager->GetParticleManager()->SetUseSph(false);
		m_particle_simulator_data->SetInitSettingParticleSystem(m_resource_manager->GetParticleManager()->GetParticleSimulator()->GetInitSettingParticleSystem());
		m_particle_simulator_data->SetParticleProperty(m_resource_manager->GetParticleManager()->GetParticleSimulator()->GetParticleProperty());
		m_particle_simulator_data->Shutdown();
		m_particle_simulator_widget->Shutdown();
	}

	void ParticleSimulatorUnload()
	{
		m_resource_manager->GetParticleManager()->ShutdownParticleSimulator();

		delete m_particle_simulator_data;
		m_particle_simulator_data = nullptr;
		delete m_particle_simulator_widget;
		m_particle_simulator_widget = nullptr;

		//m_resource_manager->GetObjectManager()->RemoveObject("PS_setting_box");
  //  m_resource_manager->GetObjectManager()->RemoveObject("PS_boundary_box");
	}

}