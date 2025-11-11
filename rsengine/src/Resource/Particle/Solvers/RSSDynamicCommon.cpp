#include "pch.h"
#include "../RSParticleSimulator.h"

#include <numbers>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/State/RSState.h"
#include "Util/RSUtilFunctions.h"


namespace RS_Particle
{
	void RSParticleSimulator::CreateDynamicParticle()
	{
		for (unsigned int index = 0; index < m_particle_count; index += 1)
		{
			instanceIDs[index] = index;
			// Create the particles by index
			m_positions[index] = glm::vec4(0.0f);
			m_velocities[index] = glm::vec4(0.0f);
			m_accelerations[index] = glm::vec4(0.0f, 0.0f, 0.0f, -1.0f);
		}
	}

	void RSParticleSimulator::UpdateInflowParticleCount()
	{
		// Get Inflow ssbo
		if (m_inflow_ssbo)
		{
			const size_t current_count_offset = m_inflow_data.m_inflow_positions.size() * sizeof(glm::vec4) + m_inflow_data.
                                          m_inflow_using.size() * sizeof(glm::uvec4);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_inflow_ssbo);
			const auto inflow_count_data = static_cast<unsigned int*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, current_count_offset, sizeof(unsigned int),GL_MAP_READ_BIT));
			if (inflow_count_data)
				m_inflow_data.current_count = *inflow_count_data;

			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}
	}

  void RSParticleSimulator::CreateInFlowBuffer()
  {
    glGenBuffers(1, &m_inflow_ssbo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_inflow_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RSInflowData), &m_inflow_data, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_inflow_ssbo);
  }

	void RSParticleSimulator::SetDynamicParticleField()
	{
		m_space = m_init_setting.init_particle_setting_space;

		m_particle_count = m_init_setting.m_inflow_max_count;

		m_valid_particle_count = true;

		if (m_solver_type == RSSolverType::R_STATIC_GRID || m_solver_type == RSSolverType::R_DYNAMIC_GRID)
		{
			m_grid_count = static_cast<unsigned int>(m_init_setting.init_grid_resolution.x) * m_init_setting.
                     init_grid_resolution.y * m_init_setting.init_grid_resolution.z;
			m_grid_size = m_init_setting.boundary_max - m_init_setting.boundary_min;
			m_max_grid_count = m_particle_count * static_cast<unsigned int>(m_init_setting.grid_max_count_per_max);
		}

		m_particle_property.particle_radius = m_space * 0.5f;
		m_particle_property.smoothing_length = m_space * 2.0f;

		// V = 4/3 * pi * r^3
		const float volume = 4.0f / 3.0f * std::numbers::pi_v<float> * m_particle_property.particle_radius *
                         m_particle_property.particle_radius * m_particle_property.particle_radius;
		// mass = volume * density
		m_particle_property.mass = m_particle_property.density * volume;
		m_inflow_data.max_particle_count = m_init_setting.m_inflow_max_count;
		m_inflow_data.current_count = 0;
	}


	void RSParticleSimulator::UpdateInOutInform()
	{
		if (const auto inflow_sphere_object = RSResourceManager::GetInstance()->GetObjectManager()->
                                                                            GetObject(INFLOW_SPHERE_OBJECT_NAME))
		{
			m_init_setting.m_inflow_position = inflow_sphere_object->GetTransform()->GetPosition();
			m_init_setting.m_inflow_radius = inflow_sphere_object->GetTransform()->GetScale().x;
			b_inflow_position_changed = true;
		}

		if (const auto inflow_zone_object = RSResourceManager::GetInstance()->GetObjectManager()->
                                                                          GetObject(INFLOW_ZONE_OBJECT_NAME))
			m_init_setting.m_inflow_zone_size = inflow_zone_object->GetTransform()->GetScale();


		if (const auto outflow_zone_object = RSResourceManager::GetInstance()->GetObjectManager()->
                                                                           GetObject(OUTFLOW_ZONE_OBJECT_NAME))
		{
			m_init_setting.m_outflow_position = outflow_zone_object->GetTransform()->GetPosition();
			m_init_setting.m_outflow_zone_size = outflow_zone_object->GetTransform()->GetScale();

		}
	}

}