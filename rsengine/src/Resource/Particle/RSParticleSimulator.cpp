#include "pch.h"
#include "RSParticleSimulator.h"

#include <numbers>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/State/RSState.h"
#include "Util/RSUtilFunctions.h"
#include "Systems/Timer/RSTimer.h"

namespace RS_Particle
{
	using namespace RS_Shader;

	RSParticleSimulator::RSParticleSimulator()
	{
		m_compute_shader_name = RS_PipelineList::RSComputeShaderNames::SPH_TRIPLE_BUFFERING;
		b_playing = false;
	}

	RSParticleSimulator::~RSParticleSimulator() = default;

	void RSParticleSimulator::Initialize()
	{
		switch (m_solver_type)
		{
    case RSSolverType::R_NONE:
			break;
      case RSSolverType::R_STATIC_BRUTE_FORCE:
      case RSSolverType::R_STATIC_GRID:
      case RSSolverType::R_STATIC_KDTREE:
			case RSSolverType::R_STATIC_OCTREE:
			case RSSolverType::R_TRIPLE_BUFFERING_TEST:
			{
				CreateStaticFlowObjects();
				SetParticleField();

				if (m_valid_particle_count)
				{
					ResizeParticleData();
					CreateStaticParticle();
					CreateParticleBuffer();

				}
				break;
			}
      case RSSolverType::R_DYNAMIC_BRUTE_FORCE:
      case RSSolverType::R_DYNAMIC_GRID:
      case RSSolverType::R_DYNAMIC_KDTREE:
      case RSSolverType::R_DYNAMIC_OCTREE:
      {
				CreateInOutFlowObjects();
				SetDynamicParticleField();
				SetInflowGridByDirection();
				ResizeParticleData();
				CreateDynamicParticle();
				CreateParticleBuffer();
				CreateInFlowBuffer();

				break;
      }
		}


		ActivateProperties();
		b_playing = true;
		m_target_legend_texture = RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT(LEGEND_TEXTURE_NAME);

		SetDividedValue(0);
		
	}

	void RSParticleSimulator::Shutdown()
	{
		ResetAllBuffers();
		DeallocateParticle();
		SyncDataFromObjects();
		RemoveToolObjects();

	  m_target_legend_texture = 0;

	}

	void RSParticleSimulator::Update(const float dt)
	{
		if (b_playing)
		{
			switch (m_solver_type)
			{
				case RSSolverType::R_NONE:
				{
          break;
				}
				case RSSolverType::R_STATIC_BRUTE_FORCE:
				{
					ComputeStaticBruteForce(dt);
					break;
				}
        case RSSolverType::R_STATIC_GRID:
        {
					ComputeStaticGrid(dt);
					break;
        }
				case RSSolverType::R_TRIPLE_BUFFERING_TEST:
				{

					ComputeTripleBufferingTest(dt);
					break;
				}
				case RSSolverType::R_DYNAMIC_BRUTE_FORCE:
				{
          ComputeDynamicBruteForce(dt);
					break;
				}
        case RSSolverType::R_STATIC_KDTREE:
        case RSSolverType::R_STATIC_OCTREE:
        case RSSolverType::R_DYNAMIC_GRID:
        case RSSolverType::R_DYNAMIC_KDTREE:
        case RSSolverType::R_DYNAMIC_OCTREE:
					break;
			}

		}

	}

	void RSParticleSimulator::ForwardDraw()
	{
		if (!IsValidParticleCount())
			return;
#if 1
		//glPointSize(15.0f);
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::PARTICLE_RENDER);

		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "view",
                                                                  RSResourceManager::GetInstance()->GetCamera()->
                                                                  GetView());
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "projection",
                                                                  RSResourceManager::GetInstance()->GetCamera()->
                                                                  GetProj());
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "camera_position",
                                                                  RSResourceManager::GetInstance()->GetCamera()->
                                                                  GetPosition());
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER,
                                                                  "point_min_point_type",
                                                                  glm::vec4(m_clipping_PMP.x, m_clipping_PMP.y,
                                                                            m_clipping_PMP.z, GetClippingTypeF()));
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER,
                                                                  "vector_max_radius_inout",
                                                                  glm::vec4(m_clipping_VMR.x, m_clipping_VMR.y,
                                                                            m_clipping_VMR.z, GetClippingAreaF()));
		if (GetDividedValue() == 0)
		{
			//glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("legend"));
			glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("legend"));
			glBindTexture(GL_TEXTURE_2D, m_target_legend_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("jordy_alpha"));
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("legend_divided"));
			glBindTexture(GL_TEXTURE_2D, m_target_legend_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("jordy_alpha"));
		}
		if (b_any_render_property_changed)
		{
			// Get window diagonal size
			const float window_size = static_cast<float>(sqrt(
				RS_Application::mRSAPPLICATION->GetWidth() * RS_Application::mRSAPPLICATION->GetWidth() +
				RS_Application::mRSAPPLICATION->GetHeight() * RS_Application::mRSAPPLICATION->GetHeight()));
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "use_legend",
                                                                    b_use_legend);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER,
                                                                    "use_texture_second", b_use_texture);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "max_value",
                                                                    value_max_legend);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "min_value",
                                                                    value_min_legend);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "screen_size",
                                                                    window_size);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "color_type",
                                                                    GetRenderPropertyTypeI());
			
			SetTextShouldUpdate(true);
			b_any_render_property_changed = false;
		}

		glBindVertexArray(m_vao);
		glDrawArraysInstanced(GL_POINTS, 0, 1, m_particle_count);
		glBindVertexArray(0);

#endif

		//switch (m_particle_rendering_type)
		//{
		//	case RSParticleRenderingType::R_NONE:
		//	{
		//		// single color
		//		// Change RSShader to color shader
		//		// uniform color
		//		break;
		//	}

		//	// vec3 data
		//	case RSParticleRenderingType::R_VELOCITY:
		//	{
		//		// vec3 velocity
		//		// Change RSShader to velocity shader
		//		// Set velocities buffer
		//	}

	}

	void RSParticleSimulator::DeferredDraw()
	{
		if (!IsValidParticleCount())
			return;

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_PARTICLE_RENDER);

		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "view",
																																	RSResourceManager::GetInstance()->GetCamera()->
																																	GetView());
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "projection",
																																	RSResourceManager::GetInstance()->GetCamera()->
																																	GetProj());
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "camera_position",
																																	RSResourceManager::GetInstance()->GetCamera()->
																																	GetPosition());
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER,
																																	"point_min_point_type",
																																	glm::vec4(m_clipping_PMP.x, m_clipping_PMP.y,
																																		m_clipping_PMP.z, GetClippingTypeF()));
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER,
																																	"vector_max_radius_inout",
																																	glm::vec4(m_clipping_VMR.x, m_clipping_VMR.y,
																																		m_clipping_VMR.z, GetClippingAreaF()));
		// near, far
    const auto camera = RSResourceManager::GetInstance()->GetCamera();
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "near",
                                                                  camera->GetNear());
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "far",
                                                                  camera->GetFar());

		if (GetDividedValue() == 0)
		{
			//glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("legend"));
			glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("legend"));
			glBindTexture(GL_TEXTURE_2D, m_target_legend_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("jordy_alpha"));
		}
		else
		{
			glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("legend_divided"));
			glBindTexture(GL_TEXTURE_2D, m_target_legend_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("jordy_alpha"));
		}
		if (b_any_render_property_changed)
		{
			// Get window diagonal size
			const float window_size = static_cast<float>(sqrt(
				RS_Application::mRSAPPLICATION->GetWidth() * RS_Application::mRSAPPLICATION->GetWidth() +
				RS_Application::mRSAPPLICATION->GetHeight() * RS_Application::mRSAPPLICATION->GetHeight()));
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "use_legend",
																																		b_use_legend);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER,
																																		"use_texture_second", b_use_texture);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "max_value",
																																		value_max_legend);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "min_value",
																																		value_min_legend);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "screen_size",
																																		window_size);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "color_type",
																																		GetRenderPropertyTypeI());

			SetTextShouldUpdate(true);
		}

		glBindVertexArray(m_vao);
		glDrawArraysInstanced(GL_POINTS, 0, 1, m_particle_count);
		glBindVertexArray(0);

	}


	void RSParticleSimulator::ResetAllBuffers()
	{
		// Reset all buffers
		if(m_position_ssbo)
			glDeleteBuffers(1, &m_position_ssbo);
		if(m_velocity_ssbo)
			glDeleteBuffers(1, &m_velocity_ssbo);
		if(m_acceleration_ssbo)
			glDeleteBuffers(1, &m_acceleration_ssbo);

		if(m_grid_ssbo)
			glDeleteBuffers(1, &m_grid_ssbo);
		if (m_grid_count_ssbo)
			glDeleteBuffers(1, &m_grid_count_ssbo);

		if(m_inflow_ssbo)
      glDeleteBuffers(1, &m_inflow_ssbo);

		if(m_mesh_collision_ssbo)
			glDeleteBuffers(1, &m_mesh_collision_ssbo);


		if(m_density_ssbo)
			glDeleteBuffers(1, &m_density_ssbo);
		if(m_pressure_ssbo)
			glDeleteBuffers(1, &m_pressure_ssbo);
		if(m_torque_ssbo)
			glDeleteBuffers(1, &m_torque_ssbo);
		if(m_stress_ssbo)
			glDeleteBuffers(1, &m_stress_ssbo);
		if (m_vorticity_ssbo)
			glDeleteBuffers(1, &m_vorticity_ssbo);
		if (m_viscosity_ssbo)
			glDeleteBuffers(1, &m_viscosity_ssbo);
		if (m_strain_ssbo)
			glDeleteBuffers(1, &m_strain_ssbo);

		if(m_vao)
			glDeleteVertexArrays(1, &m_vao);
		if(m_instance_position_vbo)
			glDeleteBuffers(1, &m_instance_position_vbo);
		if(m_instance_value_vbo)
			glDeleteBuffers(1, &m_instance_value_vbo);

		m_position_ssbo = 0;
		m_velocity_ssbo = 0;
		m_acceleration_ssbo = 0;
		m_inflow_ssbo = 0;
		m_mesh_collision_ssbo = 0;
		m_density_ssbo = 0;
		m_pressure_ssbo = 0;
		m_torque_ssbo = 0;
		m_stress_ssbo = 0;
		m_vorticity_ssbo = 0;
		m_viscosity_ssbo = 0;
		m_strain_ssbo = 0;

		m_vao = 0;
		m_instance_position_vbo = 0;
		m_instance_value_vbo = 0;

	}


	void RSParticleSimulator::SetHideObjectsStartToSolve() const
  {
    const auto object_manager = RSResourceManager::GetInstance()->GetObjectManager();
		switch(m_solver_type)
		{
    case RSSolverType::R_NONE:
			break;
    case RSSolverType::R_STATIC_BRUTE_FORCE:
    case RSSolverType::R_STATIC_GRID:
    case RSSolverType::R_STATIC_KDTREE:
    case RSSolverType::R_STATIC_OCTREE:
    case RSSolverType::R_TRIPLE_BUFFERING_TEST:
		{
			RS_ASSERT(object_manager->GetObject(BOUNDARY_BOX_OBJECT_NAME) != nullptr, "boundary box object is null...");
			object_manager->GetObject(BOUNDARY_BOX_OBJECT_NAME)->SetVisible(false);
			RS_ASSERT(object_manager->GetObject(INITIAL_ZONE_OBJECT_NAME) != nullptr, "initial zone object is null...");
			object_manager->GetObject(INITIAL_ZONE_OBJECT_NAME)->SetVisible(false);
			break;
		}
    case RSSolverType::R_DYNAMIC_BRUTE_FORCE:
    case RSSolverType::R_DYNAMIC_GRID:
    case RSSolverType::R_DYNAMIC_KDTREE:
    case RSSolverType::R_DYNAMIC_OCTREE:
		{
      //RS_ASSERT(object_manager->GetObject(INFLOW_SPHERE_OBJECT_NAME) != nullptr, "inflow sphere object is null...");
      //object_manager->GetObject(INFLOW_SPHERE_OBJECT_NAME)->SetVisible(false);
      RS_ASSERT(object_manager->GetObject(INFLOW_ZONE_OBJECT_NAME) != nullptr, "inflow zone object is null...");
      object_manager->GetObject(INFLOW_ZONE_OBJECT_NAME)->SetVisible(false);
      RS_ASSERT(object_manager->GetObject(OUTFLOW_ZONE_OBJECT_NAME) != nullptr, "outflow zone object is null...");
      object_manager->GetObject(OUTFLOW_ZONE_OBJECT_NAME)->SetVisible(false);
			break;
		}
		}
	}

	void RSParticleSimulator::SyncDataFromObjects()
	{
    // Sync the data from the objects
		// Find the object with the name.
		const auto object_manager = RSResourceManager::GetInstance()->GetObjectManager();

    // Boundary box
    if (const auto boundary_box = object_manager->GetObject(BOUNDARY_BOX_OBJECT_NAME))
		{
			const glm::vec3 boundary_box_min = boundary_box->GetTransform()->GetPosition() - 
				                                 boundary_box->GetTransform()->GetScale() * 0.5f;
			const glm::vec3 boundary_box_max = boundary_box->GetTransform()->GetPosition() + 
				                                 boundary_box->GetTransform()->GetScale() * 0.5f;

			m_init_setting.boundary_max = boundary_box_max;
			m_init_setting.boundary_min = boundary_box_min;
		}

		// Initial zone box
    if (const auto setting_box = object_manager->GetObject(INITIAL_ZONE_OBJECT_NAME))
		{
			const glm::vec3 setting_box_min = setting_box->GetTransform()->GetPosition() - 
				                                setting_box->GetTransform()->GetScale() * 0.5f;
			const glm::vec3 setting_box_max = setting_box->GetTransform()->GetPosition() + 
										                    setting_box->GetTransform()->GetScale() * 0.5f;

			m_init_setting.init_particle_setting_min = setting_box_min;
			m_init_setting.init_particle_setting_max = setting_box_max;
		}


    if (const auto inflow_sphere_object = object_manager->GetObject(INFLOW_SPHERE_OBJECT_NAME))
		{
			m_init_setting.m_inflow_position = inflow_sphere_object->GetTransform()->GetPosition();
			m_init_setting.m_inflow_radius = inflow_sphere_object->GetTransform()->GetScale().x;
		}
    if(const auto inflow_zone_object = object_manager->GetObject(INFLOW_ZONE_OBJECT_NAME))
			m_init_setting.m_inflow_zone_size = inflow_zone_object->GetTransform()->GetScale();

    if (const auto outflow_zone_object = object_manager->GetObject(OUTFLOW_ZONE_OBJECT_NAME))
		{
			m_init_setting.m_outflow_position = outflow_zone_object->GetTransform()->GetPosition();
			m_init_setting.m_outflow_zone_size = outflow_zone_object->GetTransform()->GetScale();
		}

	}


	void RSParticleSimulator::ComputeTripleBufferingTest(float dt)
	{
		//// TEST For Triple Buffering
		//RS_Object::RSObject* player = RSResourceManager::GetInstance()->GetObjectManager()->GetObject("Player");

		//int next_buffer = (m_current_buffer + 1) % 3;

		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_position_ssbo[m_current_buffer]);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_position_ssbo[next_buffer]);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_velocity_ssbo[m_current_buffer]);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_velocity_ssbo[next_buffer]);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_acceleration_ssbo[m_current_buffer]);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_acceleration_ssbo[next_buffer]);

		//// Calculate the new position of the particles with Compute shader
		//RSResourceManager::GetInstance()->GetShaderManager()->Use(m_compute_shader_name);
		//RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "deltaTime", dt);

		//if (b_init_boundary_condition_changed)
		//{
		//	RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "box_min", m_init_setting.boundary_min);
		//	RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "box_max", m_init_setting.boundary_max);
		//	b_init_boundary_condition_changed = false;
		//}

		//if (b_any_calculate_property_changed)
		//{
		//	RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "particle_radius", m_particle_property.particle_radius);

		//	b_any_calculate_property_changed = false;
		//}

		//if (player)
		//{
		//	glm::vec3 player_position = player->GetTransform()->GetPosition();
		//	RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "circle_center", player_position);
		//	RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "circle_radius", 2.0f);
		//	RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "is_play", true);
		//}
		//else
		//{
		//	RSResourceManager::GetInstance()->GetShaderManager()->SetData(m_compute_shader_name, "is_play", false);
		//}


		//// Set the particle data

		//glDispatchCompute((m_particle_count + 1023) / 1024, 1, 1);
		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		////int get_count_x = 0, get_count_y = 0, get_count_z = 0;
		////glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,0 , &get_count_x);
		////glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,1 , &get_count_y);
		////glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,2 , &get_count_z);
		////std::cout << "Max Compute Work Group Count X : " << get_count_x << std::endl;
		////std::cout << "Max Compute Work Group Count Y : " << get_count_y << std::endl;
		////std::cout << "Max Compute Work Group Count Z : " << get_count_z << std::endl;
		//m_current_buffer = next_buffer;
	}




	void RSParticleSimulator::UpdateInitProperty()
	{
		b_init_boundary_condition_changed = true;
	}

	void RSParticleSimulator::UpdateCalculateProperty()
	{
		b_any_calculate_property_changed = true;
	}

	void RSParticleSimulator::UpdateRenderProperty()
	{
		b_any_render_property_changed = true;
	}



	void RSParticleSimulator::DeallocateParticle()
	{
		instanceIDs.clear();

		m_positions.clear();
		m_velocities.clear();
		m_accelerations.clear();

		m_grid_debug.clear();
		m_grid_count_debug.clear();

		m_mesh_vertices.clear();
		m_mesh_vertices.shrink_to_fit();

		//m_density.clear();
		//m_pressure.clear();
		//m_torque.clear();
		//m_stress.clear();
		//m_vorticity.clear();
		//m_viscosity.clear();
		//m_strain.clear();
	}



	void RSParticleSimulator::SetLegendTextureId(const unsigned int texture_)
	{
    m_target_legend_texture = texture_;
	}

	void RSParticleSimulator::ActivateProperties()
	{
		b_any_calculate_property_changed = true;
		b_init_boundary_condition_changed = true;
		b_any_collision_data_changed = true;
	}
}