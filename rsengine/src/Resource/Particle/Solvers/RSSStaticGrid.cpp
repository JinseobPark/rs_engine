#include "pch.h"
#include "../RSParticleSimulator.h"

#include "Resource/Manager/RSResourceManager.h"
#include "Util/RSUtilFunctions.h"


namespace RS_Particle
{
	void RSParticleSimulator::ComputeStaticGrid(const float dt)
	{

		InitializeStaticGrid();

		TransferMeshCollisionData();

		ComputeStaticGrid_Batch();

		ComputeStaticGrid_Density();

		ComputeStaticGrid_Force();

		ComputeStaticGrid_Post(dt);

		if (b_call_calculate_minmax)
			CalculateMinMaxValue();
	}


	void RSParticleSimulator::InitializeStaticGrid() const
	{
		// Grid initializer 
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_GRID_INIT);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_INIT,
                                                                  "grid_size", m_grid_size);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_INIT,
                                                                  "grid_resolution",
                                                                  m_init_setting.init_grid_resolution);

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void RSParticleSimulator::ComputeStaticGrid_Batch() const
	{
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_GRID_BATCH);
		if (b_any_calculate_property_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_BATCH, "grid_size",
                                                m_grid_size);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_BATCH,
                                                "grid_resolution", m_init_setting.init_grid_resolution);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_BATCH,
                                                "grid_min_boundary", m_init_setting.boundary_min);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_BATCH,
                                                "particle_max_count_cell", m_max_grid_count);
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


		// SPH Grid Batch Debug
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_grid_ssbo);
		//unsigned int* grid_data_debug = (unsigned int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_grid_count * sizeof(unsigned int) * m_max_grid_count, GL_MAP_READ_BIT);
		//if(grid_data_debug)
		//	for (int i = 0; i < m_grid_count * m_max_grid_count; i++)
		//		m_grid_debug[i] = grid_data_debug[i];
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// SPH Grid Count Debug
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_grid_count_ssbo);
		//int* grid_count_data_debug = (int*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_grid_count * sizeof(int), GL_MAP_READ_BIT);
		//if (grid_count_data_debug)
		//	for (int i = 0; i < m_grid_count; i++)
		//		m_grid_count_debug[i] = grid_count_data_debug[i];
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		//		
	}

	void RSParticleSimulator::ComputeStaticGrid_Density() const
	{
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY);
		if (b_any_calculate_property_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY, "grid_size",
                                                m_grid_size);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY,
                                                "grid_resolution", m_init_setting.init_grid_resolution);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY,
                                                "grid_min_boundary", m_init_setting.boundary_min);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY,
                                                "particle_max_count_cell", m_max_grid_count);

			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY,
                                                "particle_radius", m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY,
                                                "smoothing_length", m_particle_property.smoothing_length);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY,
                                                "particle_mass", m_particle_property.mass);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY, "u_density_0",
                                                m_particle_property.density);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_DENSITY,
                                                "u_gas_constant", m_particle_property.gas_constant);
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	}

	void RSParticleSimulator::ComputeStaticGrid_Force()
	{
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC);
		if (b_any_calculate_property_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "grid_size", m_grid_size);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "grid_resolution",
                                                                    m_init_setting.init_grid_resolution);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "grid_min_boundary", m_init_setting.boundary_min);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "particle_max_count_cell", m_max_grid_count);

			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "u_particle_radius",
                                                                    m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "u_particle_mass", m_particle_property.mass);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "smoothing_length",
                                                                    m_particle_property.smoothing_length);

			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "u_viscosity", m_particle_property.viscosity);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "u_density_0", m_particle_property.density);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_ACC,
                                                                    "u_gravity", m_particle_property.gravity);
			b_any_calculate_property_changed = false;
			SetTextShouldUpdate(true);
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	}

	void RSParticleSimulator::ComputeStaticGrid_Post(float dt)
	{
		RS_Object::RSObject* player = RSResourceManager::GetInstance()->GetObjectManager()->GetObject("Player");

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_GRID_POST);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_POST,
                                                                  "deltaTime", dt);
		if (player != nullptr)
		{
			glm::vec3 player_position = player->GetTransform()->GetPosition();
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_POST, "circle_center",
                                                player_position);
		}

		if (b_init_boundary_condition_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_POST,
                                                "u_particle_radius", m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_POST, "circle_radius",
                                                2.0f);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_POST, "box_min",
                                                m_init_setting.boundary_min);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_GRID_POST, "box_max",
                                                m_init_setting.boundary_max);
			b_init_boundary_condition_changed = false;
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}


	void RSParticleSimulator::CreateGridBuffer()
	{
		// Create Grid SSBO
		glGenBuffers(1, &m_grid_ssbo);
		glGenBuffers(1, &m_grid_count_ssbo);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_grid_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_grid_count * sizeof(int) * m_max_grid_count, nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_grid_count_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_grid_count * sizeof(int), nullptr, GL_DYNAMIC_DRAW);

		// First, fill the buffer with the data
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_grid_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_grid_count * sizeof(int) * m_max_grid_count, m_grid_debug.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_grid_count_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_grid_count * sizeof(int), m_grid_count_debug.data());

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_grid_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_grid_count_ssbo);
	}

}