#include "pch.h"
#include "../RSParticleSimulator.h"

#include <numbers>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/State/RSState.h"
#include "Util/RSUtilFunctions.h"


namespace RS_Particle
{

	void RSParticleSimulator::ComputeStaticBruteForce(const float dt)
	{
		TransferMeshCollisionData();

		ComputeStaticBruteForce_Density();
		ComputeStaticBruteForce_Force(dt);
		ComputeStaticBruteForce_Post(dt);

		if (b_call_calculate_minmax)
			CalculateMinMaxValue();
	}


	void RSParticleSimulator::ComputeStaticBruteForce_Density() const
	{
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_CMU_DENSITY);
		if (b_any_calculate_property_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_DENSITY,
                                                "particle_radius", m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_DENSITY,
                                                "smoothing_length", m_particle_property.smoothing_length);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_DENSITY, "particle_mass",
                                                m_particle_property.mass);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_DENSITY, "u_density_0",
                                                m_particle_property.density);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_DENSITY,
                                                "u_gas_constant", m_particle_property.gas_constant);
		}
		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	}

	void RSParticleSimulator::ComputeStaticBruteForce_Force(const float dt)
	{
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE,
                                                                  "delta_time", dt);
		if (b_any_calculate_property_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE,
                                                "u_particle_radius", m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE,
                                                "smoothing_length", m_particle_property.smoothing_length);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE, "u_particle_mass",
                                                m_particle_property.mass);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE, "u_viscosity",
                                                m_particle_property.viscosity);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE, "u_density_0",
                                                m_particle_property.density);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_FORCE, "u_gravity",
                                                m_particle_property.gravity);
			b_any_calculate_property_changed = false;
			SetTextShouldUpdate(true);
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	}

	void RSParticleSimulator::ComputeStaticBruteForce_Post(const float dt)
	{
		RS_Object::RSObject* player = RSResourceManager::GetInstance()->GetObjectManager()->GetObject("Player");

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_CMU_POST);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_POST,
                                                                  "deltaTime", dt);

		if (player != nullptr)
		{
			const glm::vec3 player_position = player->GetTransform()->GetPosition();
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_POST,
                                                                    "circle_center", player_position);
		}

		if (b_init_boundary_condition_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_POST,
                                                                    "u_particle_radius",
                                                                    m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_POST,
                                                                    "circle_radius", 2.0f);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_POST,
                                                                    "box_min", m_init_setting.boundary_min);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_CMU_POST,
                                                                    "box_max", m_init_setting.boundary_max);

			b_init_boundary_condition_changed = false;
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void RSParticleSimulator::CreateStaticFlowObjects() const
	{
		const auto m_resource_manager = RSResourceManager::GetInstance();
		{ // Particle boundary Box
			const auto boundary_box = m_resource_manager->GetObjectManager()->CreateObject(BOUNDARY_BOX_OBJECT_NAME);

			const auto boundary_box_model = new RS_Component::RSModel();
			boundary_box_model->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			boundary_box->AddComponent(boundary_box_model);

			const auto cube_mesh = new RS_Mesh::RSMesh();
			cube_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Cube"));
			cube_mesh->SetIsTexture(true);

			boundary_box_model->SetMesh(cube_mesh);
			boundary_box->SetRenderLayer(RS_PipelineList::RSRenderLayer::R_LINE_OPAQUE);

			const glm::vec3 boundary_box_position = m_init_setting.boundary_min + (
                                                m_init_setting.boundary_max - m_init_setting.boundary_min) * 0.5f;
			const glm::vec3 boundary_box_scale = m_init_setting.boundary_max - m_init_setting.boundary_min;
			boundary_box->GetTransform()->SetPosition(boundary_box_position);
			boundary_box->GetTransform()->SetScale(boundary_box_scale);
		}

		{ // Particle init setting box
			const auto setting_box = m_resource_manager->GetObjectManager()->CreateObject(INITIAL_ZONE_OBJECT_NAME);

			const auto setting_box_model = new RS_Component::RSModel();
			setting_box->AddComponent(setting_box_model);

			const auto cube_mesh = new RS_Mesh::RSMesh();
			cube_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Cube"));
			cube_mesh->SetIsTexture(true);

			setting_box_model->SetMesh(cube_mesh);
			setting_box->SetRenderLayer(RS_PipelineList::RSRenderLayer::R_LINE_OPAQUE);

			const glm::vec3 setting_box_position = m_init_setting.init_particle_setting_min + (
                                               m_init_setting.init_particle_setting_max - m_init_setting.
                                               init_particle_setting_min) * 0.5f;
			const glm::vec3 setting_box_scale = 
				        m_init_setting.init_particle_setting_max - m_init_setting.init_particle_setting_min;
			setting_box->GetTransform()->SetPosition(setting_box_position);
			setting_box->GetTransform()->SetScale(setting_box_scale);
		}
	}

}