#include "pch.h"
#include "../RSParticleSimulator.h"

#include <algorithm>
#include <numbers>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/State/RSState.h"
#include "Util/RSUtilFunctions.h"


namespace RS_Particle
{
	void RSParticleSimulator::CreateStaticParticle()
	{
		// Create particles with the given field

		unsigned int x_count = static_cast<unsigned int>((m_cube_max.x - m_cube_min.x) / m_space);
		unsigned int y_count = static_cast<unsigned int>((m_cube_max.y - m_cube_min.y) / m_space);
		unsigned int z_count = static_cast<unsigned int>((m_cube_max.z - m_cube_min.z) / m_space);

#if 1
		for (unsigned int x = 0; x < x_count; x += 1)
		{
			for (unsigned int y = 0; y < y_count; y += 1)
			{
				for (unsigned int z = 0; z < z_count; z += 1)
				{
					instanceIDs[x * y_count * z_count + y * z_count + z] = (unsigned int)x * y_count * z_count + y * z_count + z;
					// Create the particles by index
					m_positions[x * y_count * z_count + y * z_count + z] = glm::vec4(x * m_space + m_cube_min.x,
                                                                           y * m_space + m_cube_min.y,
                                                                           z * m_space + m_cube_min.z,
                                                                           m_particle_property.density);

					// random velocity -10 ~ 10 each axis
					//m_velocities[x * y_count * z_count + y * z_count + z] = glm::vec4((rand() % 20 - 10) * 0.2f, (rand() % 20 - 10) * 1.0f, (rand() % 20 - 10) * 0.2f, m_pressure);
					m_velocities[x * y_count * z_count + y * z_count + z] = glm::vec4(0.0f);

					//m_velocities[(unsigned int)x * y_count * z_count + y * z_count + z] = glm::vec4(0.0f);
					m_accelerations[(unsigned int)x * y_count * z_count + y * z_count + z] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
				}
			}
		}
#endif
	}

	void RSParticleSimulator::UpdateAllProperty()
	{
		UpdateInitProperty();
		UpdateCalculateProperty();
		UpdateRenderProperty();
	}


	void RSParticleSimulator::CalculateMinMaxValue()
	{
		// Initialize the min max value

		value_min_legend = FLT_MAX;
		value_max_legend = FLT_MIN;

		switch (m_render_property_type)
		{
		case RSRenderPropertyType::R_COLOR:
		{
			// nothing to do
			break;
		}
		case RSRenderPropertyType::R_VELOCITY:
		{
			// Get velocity Data from SSBO
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velocity_ssbo);
			const auto velocity_data = static_cast<glm::vec4*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4),GL_MAP_READ_BIT));

			if (velocity_data)
				for (size_t i = 0; i < m_particle_count; i++)
				{
					m_velocities[i]        = velocity_data[i];
					const float vel_length = glm::length(glm::vec3(velocity_data[i].x, velocity_data[i].y, velocity_data[i].z));
          value_min_legend       = std::min(vel_length, value_min_legend);
          value_max_legend       = std::max(vel_length, value_max_legend);
        }
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			break;
		}
		case RSRenderPropertyType::R_ACCELERATION:
		{
			// Get acceleration Data from SSBO
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_acceleration_ssbo);
			const auto acceleration_data = static_cast<glm::vec4*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4),GL_MAP_READ_BIT));
			if (acceleration_data)
				for (size_t i = 0; i < m_particle_count; i++)
				{
					m_accelerations[i] = acceleration_data[i];
					float acc_length = glm::length(glm::vec3(acceleration_data[i].x, acceleration_data[i].y, acceleration_data[i].z));
          value_min_legend = std::min(acc_length, value_min_legend);
          value_max_legend = std::max(acc_length, value_max_legend);
        }
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			break;
		}
		case RSRenderPropertyType::R_DENSITY:
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
			const glm::vec4* position_data = static_cast<glm::vec4*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4),GL_MAP_READ_BIT));
			if (position_data)
				for (size_t i = 0; i < m_particle_count; i++)
				{
					m_positions[i]   = position_data[i];
          value_min_legend = std::min(position_data[i].w, value_min_legend);
          value_max_legend = std::max(position_data[i].w, value_max_legend);
        }
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

			break;
		}
		case RSRenderPropertyType::R_PRESSURE:
		{
			// Get velocity Data from SSBO
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velocity_ssbo);
			const auto velocity_data = static_cast<glm::vec4*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4),GL_MAP_READ_BIT));
			if (velocity_data)
				for (size_t i = 0; i < m_particle_count; i++)
				{
					m_velocities[i] = velocity_data[i];

          value_min_legend = std::min(velocity_data[i].w, value_min_legend);
          value_max_legend = std::max(velocity_data[i].w, value_max_legend);
        }
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			break;
		}
		default:
			break;
		}
		b_call_calculate_minmax = false;
		SetAnyRenderPropertyChanged();
	}
	void RSParticleSimulator::CallCalculateMinMaxValue()
	{
		b_call_calculate_minmax = true;
	}


	float RSParticleSimulator::GetPointSize() const
	{
		return m_point_size;
	}
	void RSParticleSimulator::SetPointSize(const float point_size)
	{
		m_point_size = point_size;
	}
	void RSParticleSimulator::SetParticleRenderingType(const RSParticleRenderingType particle_rendering_type)
	{
		m_particle_rendering_type = particle_rendering_type;
	}
	RSParticleRenderingType RSParticleSimulator::GetParticleRenderingType() const
	{
		return m_particle_rendering_type;
	}
	float RSParticleSimulator::GetClippingTypeF()
	{
		return static_cast<float>(m_clipping_type);
	}
	int RSParticleSimulator::GetClippingTypeI()
	{
		return static_cast<int>(m_clipping_type);
	}
	void RSParticleSimulator::SetClippingType(int clipping_type_)
	{
		switch (clipping_type_)
		{
		case 0:
			m_clipping_type = RSClippingType::NONE;
			break;
		case 1:
			m_clipping_type = RSClippingType::PLANE;
			break;
		case 2:
			m_clipping_type = RSClippingType::CUBE;
			break;
		case 3:
			m_clipping_type = RSClippingType::SPHERE;
			break;
		default:
			break;
		}

	}
	float RSParticleSimulator::GetClippingAreaF()
	{
		return (m_clipping_area == RSClippingArea::INSIDE) ? 1.0f : -1.0f;
	}
	int RSParticleSimulator::GetClippingAreaI()
	{
		return  (m_clipping_area == RSClippingArea::INSIDE) ? 0 : 1;
	}
	void RSParticleSimulator::SetClippingArea(int clipping_area_)
	{
		if (clipping_area_ == 0)
			m_clipping_area = RSClippingArea::INSIDE;
		else
			m_clipping_area = RSClippingArea::OUTSIDE;
	}
	unsigned int RSParticleSimulator::GetDividedValue() const
	{
		return m_divided_value;
	}
	void RSParticleSimulator::SetDividedValue(const unsigned int divided_value)
	{
		m_divided_value = divided_value;
	}
	void RSParticleSimulator::SetUsingLegend(const bool use_legend_)
	{
		b_use_legend = use_legend_;
	}
	bool RSParticleSimulator::GetUsingLegend() const
	{
		return b_use_legend;
	}
	void RSParticleSimulator::SetUsingTexture(const bool use_texture_)
	{
		b_use_texture = use_texture_;
	}
	bool RSParticleSimulator::GetUsingTexture() const
	{
		return b_use_texture;
	}
	void RSParticleSimulator::SetNextRenderPropertyType()
	{
		SetRenderPropertyType(static_cast<RSRenderPropertyType>((static_cast<int>(m_render_property_type) + 1) % 5));
	}
	void RSParticleSimulator::SetRenderPropertyType(const RSRenderPropertyType render_property_type_)
	{
		b_call_calculate_minmax = true;
		b_any_render_property_changed = true;
		m_render_property_type = render_property_type_;
		switch (m_render_property_type)
		{
		case RSRenderPropertyType::R_COLOR:
		{
			b_use_legend = false;
			break;
		}

		case RSRenderPropertyType::R_VELOCITY:
		case RSRenderPropertyType::R_ACCELERATION:
		case RSRenderPropertyType::R_DENSITY:
		case RSRenderPropertyType::R_PRESSURE:
			b_use_legend = true;
			break;
		}
	}

	RSRenderPropertyType RSParticleSimulator::GetRenderPropertyType() const
	{
		return m_render_property_type;
	}

	int RSParticleSimulator::GetRenderPropertyTypeI() const
	{
		return static_cast<int>(m_render_property_type);
	}

	void RSParticleSimulator::SetMinValueLegend(const float value_)
	{
		value_min_legend = value_;
		b_any_render_property_changed = true;
	}

	float RSParticleSimulator::GetMinValueLegend() const
	{
		return value_min_legend;
	}

	void RSParticleSimulator::SetMaxValueLegend(const float value_)
	{
		value_max_legend = value_;
		b_any_render_property_changed = true;
	}
	float RSParticleSimulator::GetMaxValueLegend() const
	{
		return value_max_legend;
	}


	void RSParticleSimulator::GetSsboDataFromGPU()
	{
		// SSBO data mapping test for debug
		//Get Position Data from SSBO

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		const glm::vec4* position_data = static_cast<glm::vec4*>(
      glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4),GL_MAP_READ_BIT));
		if (position_data)
			for (unsigned int i = 0; i < m_particle_count; i++)
				m_positions[i] = position_data[i];
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// Get velocity Data from SSBO
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velocity_ssbo);
		const glm::vec4* velocity_data = static_cast<glm::vec4*>(
      glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4),GL_MAP_READ_BIT));
		if (velocity_data)
			for (unsigned int i = 0; i < m_particle_count; i++)
				m_velocities[i] = velocity_data[i];
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// Get acceleration Data from SSBO
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_acceleration_ssbo);
		const glm::vec4* acceleration_data = static_cast<glm::vec4*>(
      glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4),GL_MAP_READ_BIT));
		if (acceleration_data)
			for (unsigned int i = 0; i < m_particle_count; i++)
				m_accelerations[i] = acceleration_data[i];
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


		// Get mesh collision Data from SSBO
		if (m_mesh_collision_ssbo)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_mesh_collision_ssbo);
			const auto mesh_collision_data = static_cast<glm::vec4*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_mesh_vertices.size() * sizeof(glm::vec4),GL_MAP_READ_BIT));
			if (mesh_collision_data)
				for (int i = 0; i < m_mesh_vertices.size(); i++)
					m_mesh_vertices[i] = mesh_collision_data[i];
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}



		// Get Inflow ssbo
		if (m_inflow_ssbo)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_inflow_ssbo);
			const RSInflowData* inflow_data = static_cast<RSInflowData*>(
        glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(RSInflowData), GL_MAP_READ_BIT));
			if (inflow_data)
			{
				for (int i = 0; i < INFLOW_MAX_COUNT; i++)
				{
					m_inflow_data.m_inflow_positions[i] = inflow_data->m_inflow_positions[i];
					m_inflow_data.m_inflow_using[i] = inflow_data->m_inflow_using[i];

				}
				m_inflow_data.current_count = inflow_data->current_count;
			}

			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}


	}


	void RSParticleSimulator::SetParticleCount(const unsigned int particle_count)
	{
		m_particle_count = std::max(particle_count, m_max_particle_count);
	}

	void RSParticleSimulator::StartSimulation()
	{
		b_playing = true;
		SyncDataFromObjects();
	}

	void RSParticleSimulator::SetInitSettingParticleSystem(const RSInitSettingParticleSystem& init_setting_)
	{
		m_init_setting = init_setting_;
	}


	void RSParticleSimulator::SetSolverType(const RSSolverType solver_type_)
	{
		m_solver_type = solver_type_;
		RS_State::mRSSTATE->Restart();
	}

	void RSParticleSimulator::InitSetSolverType(const RSSolverType solver_type_)
	{
		m_solver_type = solver_type_;
	}

}