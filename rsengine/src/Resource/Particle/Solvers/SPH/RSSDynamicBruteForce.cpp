#include "pch.h"
#include "../../RSParticleSimulator.h"

#include <numbers>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/State/RSState.h"
#include "Util/RSUtilFunctions.h"


namespace RS_Particle
{

	void RSParticleSimulator::ComputeDynamicBruteForce(const float dt)
	{
		TransferMeshCollisionData();

		ComputeDynamicBruteForce_Status();

		ComputeDynamicBruteForce_Density();

		ComputeDynamicBruteForce_Force();

		ComputeDynamicBruteForce_Post(dt);


		if (b_call_calculate_minmax)
			CalculateMinMaxValue();

	}


	void RSParticleSimulator::ComputeDynamicBruteForce_Status()
	{
		auto m_object_manager = RSResourceManager::GetInstance()->GetObjectManager();
		RS_ASSERT(m_object_manager != nullptr, "object manager didn't linked");

		auto inflow_sphere_object = m_object_manager->GetObject(INFLOW_SPHERE_OBJECT_NAME);
		RS_ASSERT(inflow_sphere_object != nullptr, "inflow sphere is null");

		auto inflow_zone_object = m_object_manager->GetObject(INFLOW_ZONE_OBJECT_NAME);
		RS_ASSERT(inflow_zone_object != nullptr, "inflow zone is null");

		inflow_zone_object->GetTransform()->SetPosition(inflow_sphere_object->GetTransform()->GetPosition());

		auto outflow_zone_object = m_object_manager->GetObject(OUTFLOW_ZONE_OBJECT_NAME);
		RS_ASSERT(outflow_zone_object != nullptr, "outflow zone is null");

		glm::vec3 inflow_position = inflow_sphere_object->GetTransform()->GetPosition();

		glm::vec3 inflow_direction_radian = m_init_setting.m_inflow_direction;
		float inflow_direction_normalized = 1.0f / glm::length(inflow_direction_radian);

		//glm::vec3 inflow_velocity		= glm::normalize(m_init_setting.m_inflow_direction) * m_init_setting.m_inflow_speed;
		glm::vec3 inflow_velocity = inflow_direction_radian * inflow_direction_normalized * m_init_setting.m_inflow_speed;
		glm::vec3 inflow_zone_size = inflow_zone_object->GetTransform()->GetScale();
		glm::vec3 outflow_position = outflow_zone_object->GetTransform()->GetPosition();
		glm::vec3 outflow_zone_size = outflow_zone_object->GetTransform()->GetScale();

		RSResourceManager::GetInstance()->GetShaderManager()->
                                      Use(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_STATE);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_STATE,
                                              "inflow_position", inflow_position);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_STATE,
                                              "inflow_velocity", inflow_velocity);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_STATE,
                                              "inflow_zone_size", inflow_zone_size);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_STATE,
                                              "outflow_position", outflow_position);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_STATE,
                                              "outflow_zone_size", outflow_zone_size);

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		UpdateInflowDirection();

		auto inflow_transform_mat = glm::mat4(1.0f);
		//inflow_transform_mat = glm::translate(inflow_transform_mat, -inflow_position);
		inflow_transform_mat *= m_inflow_transform;
		//inflow_transform_mat = glm::translate(inflow_transform_mat, inflow_position);

		RSResourceManager::GetInstance()->GetShaderManager()->
                                      Use(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_CREATOR);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_CREATOR,
                                              "inflow_position", inflow_position);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_CREATOR,
                                              "inflow_velocity", inflow_velocity);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_CREATOR,
                                              "inflow_rotate_mat", inflow_transform_mat);

		glDispatchCompute(1, 1, 1); // MAX CREATED PARTICLE (1024) / 256 = 4
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		UpdateInflowParticleCount();
		//GetSsboDataFromGPU();
	}

	void RSParticleSimulator::ComputeDynamicBruteForce_Density() const
	{
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      Use(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY);
		if (b_any_calculate_property_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY,
                                                "particle_radius", m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY,
                                                "smoothing_length", m_particle_property.smoothing_length);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY,
                                                "particle_mass", m_particle_property.mass);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY,
                                                "u_density_0", m_particle_property.density);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY,
                                                "u_gas_constant", m_particle_property.gas_constant);
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	}

	void RSParticleSimulator::ComputeDynamicBruteForce_Force()
	{
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      Use(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE);
		if (b_any_calculate_property_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE,
                                                "u_particle_radius", m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE,
                                                "smoothing_length", m_particle_property.smoothing_length);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE,
                                                "u_particle_mass", m_particle_property.mass);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE,
                                                "u_viscosity", m_particle_property.viscosity);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE,
                                                "u_density_0", m_particle_property.density);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE,
                                                "u_gravity", m_particle_property.gravity);
			b_any_calculate_property_changed = false;
			SetTextShouldUpdate(true);
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void RSParticleSimulator::ComputeDynamicBruteForce_Post(const float dt)
	{
		RS_Object::RSObject* player = RSResourceManager::GetInstance()->GetObjectManager()->GetObject("Player");

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_POST);
		RSResourceManager::GetInstance()->GetShaderManager()->
                                      SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_POST, "deltaTime",
                                              dt);

		if (player != nullptr)
		{
			const glm::vec3 player_position = player->GetTransform()->GetPosition();
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_POST,
                                                "circle_center", player_position);
		}

		if (b_init_boundary_condition_changed)
		{
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_POST,
                                                "u_particle_radius", m_particle_property.particle_radius);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_POST,
                                                "circle_radius", 2.0f);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_POST, "box_min",
                                                m_init_setting.boundary_min);
			RSResourceManager::GetInstance()->GetShaderManager()->
                                        SetData(RS_PipelineList::RSComputeShaderNames::SPH_DYNAMIC_BT_POST, "box_max",
                                                m_init_setting.boundary_max);

			b_init_boundary_condition_changed = false;
		}

		glDispatchCompute((m_particle_count + 255) / 256, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}


  void RSParticleSimulator::SetInflowGridByDirection()
  {
    // This Function is for definition of inflow position by direction
    if (!b_inflow_direction_changed)
      return;

    // Create a circle from inflow velocity(normal) and inflow position to the inflow sphere;
    // Circle center = inflow position
    // If velocity is 0, set the velocity to minimum value with gravity.

    // Reset the inflow count.
    m_init_setting.m_inflow_count = 0;

    if (m_init_setting.m_inflow_direction == glm::vec3(0, 0, 0))
      //m_init_setting.m_inflow_direction = glm::vec3(0, -1, 0);
      m_init_setting.m_inflow_direction = glm::vec3(0, -180, 0);

    const glm::vec3 normal_inflow_direction = glm::vec3(1, 0, 0);
    //const glm::vec3 normal_inflow_direction = glm::normalize(m_init_setting.m_inflow_direction);
    //m_init_setting.m_inflow_direction = normal_inflow_direction;

    auto world_up = glm::vec3(0.0f, 1.0f, 0.0f);

    // When angle between direction and world up is vertical, the world should be changed to right.
    // And also the angle is 0, it should be changed.
    if (glm::dot(normal_inflow_direction, world_up) == 1.0f || glm::dot(normal_inflow_direction, world_up) == -1.0f)
    {
      // If the direction is vertical, change the world up to right
      world_up = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    // circle right
    const glm::vec3 c_right = glm::normalize(glm::cross(world_up, normal_inflow_direction));
    // circle up
    const glm::vec3 c_up = glm::normalize(glm::cross(c_right, normal_inflow_direction));

    // Calculate the circle center
    constexpr auto circle_center = glm::vec3(0.0f);

    // Create Grid with the circle center and radius with space value
    const int grid_size = static_cast<int>(std::floor(m_init_setting.m_inflow_radius * 0.5f / m_space));

    for (int x = -grid_size; x <= grid_size; x++)
    {
      for (int y = -grid_size; y <= grid_size; y++)
      {
        // Calculate the position
        const glm::vec3 position = circle_center + c_right * static_cast<float>(x) * m_space + c_up * static_cast<float>
                                   (y) * m_space;

        // Check the position is in the circle
        if (glm::distance(position, circle_center) > m_init_setting.m_inflow_radius * 0.5f)
          continue;

        if (m_init_setting.m_inflow_count >= INFLOW_MAX_COUNT)
        {
          m_inflow_data.inflow_creator_size = m_init_setting.m_inflow_count;
          return;
        }
        // Add the position to the inflow position
        m_inflow_data.m_inflow_positions[m_init_setting.m_inflow_count] = glm::vec4(position, 1.0);
        m_inflow_data.m_inflow_using[m_init_setting.m_inflow_count] = glm::uvec4(0);
        //m_inflow_positions[m_init_setting.m_inflow_count] = position;
        m_init_setting.m_inflow_count++;
      }
    }
    m_inflow_data.inflow_creator_size = m_init_setting.m_inflow_count;
  }

  void RSParticleSimulator::CreateInOutFlowObjects() const
  {
    { // Inflow Sphere Object
      const auto inflow_sphere_object = RSResourceManager::GetInstance()->GetObjectManager()->
                                                                          CreateObject(INFLOW_SPHERE_OBJECT_NAME);
      const auto inflow_sphere_model = new RS_Component::RSModel();
      inflow_sphere_model->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
      inflow_sphere_object->AddComponent(inflow_sphere_model);

      const auto inflow_sphere_mesh = new RS_Mesh::RSMesh();
      inflow_sphere_mesh->SetMeshData(RSResourceManager::GetInstance()->GetMeshManager()->GetMesh("Sphere"));
      inflow_sphere_mesh->SetIsTexture(true);

      inflow_sphere_model->SetMesh(inflow_sphere_mesh);
      inflow_sphere_object->SetRenderLayer(RS_PipelineList::RSRenderLayer::R_LINE_OPAQUE);

      inflow_sphere_object->GetTransform()->SetPosition(m_init_setting.m_inflow_position);
      inflow_sphere_object->GetTransform()->SetScale(glm::vec3(m_init_setting.m_inflow_radius));
    }

    { // Inflow Zone Object (cube)
      const auto inflow_zone_object = RSResourceManager::GetInstance()->GetObjectManager()->
                                                                        CreateObject(INFLOW_ZONE_OBJECT_NAME);

      const auto inflow_zone_model = new RS_Component::RSModel();
      inflow_zone_model->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
      inflow_zone_object->AddComponent(inflow_zone_model);

      const auto inflow_zone_mesh = new RS_Mesh::RSMesh();
      inflow_zone_mesh->SetMeshData(RSResourceManager::GetInstance()->GetMeshManager()->GetMesh("Cube"));
      inflow_zone_mesh->SetIsTexture(true);

      inflow_zone_model->SetMesh(inflow_zone_mesh);
      inflow_zone_object->SetRenderLayer(RS_PipelineList::RSRenderLayer::R_LINE_OPAQUE);

      inflow_zone_object->GetTransform()->SetPosition(m_init_setting.m_inflow_position);
      inflow_zone_object->GetTransform()->SetScale(m_init_setting.m_inflow_zone_size);
    }

    { // Outflow Zone Object (cube)
      const auto outflow_zone_object = RSResourceManager::GetInstance()->GetObjectManager()->
                                                                         CreateObject(OUTFLOW_ZONE_OBJECT_NAME);

      const auto outflow_zone_model = new RS_Component::RSModel();
      outflow_zone_model->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
      outflow_zone_object->AddComponent(outflow_zone_model);

      const auto outflow_zone_mesh = new RS_Mesh::RSMesh();
      outflow_zone_mesh->SetMeshData(RSResourceManager::GetInstance()->GetMeshManager()->GetMesh("Cube"));
      outflow_zone_mesh->SetIsTexture(true);

      outflow_zone_model->SetMesh(outflow_zone_mesh);
      outflow_zone_object->SetRenderLayer(RS_PipelineList::RSRenderLayer::R_LINE_OPAQUE);

      outflow_zone_object->GetTransform()->SetPosition(m_init_setting.m_outflow_position);
      outflow_zone_object->GetTransform()->SetScale(m_init_setting.m_outflow_zone_size);
    }

  }


  void RSParticleSimulator::RemoveToolObjects()
  {
    const auto object_manager = RSResourceManager::GetInstance()->GetObjectManager();

    object_manager->RemoveObject(INITIAL_ZONE_OBJECT_NAME);
    object_manager->RemoveObject(BOUNDARY_BOX_OBJECT_NAME);
    object_manager->RemoveObject(INFLOW_SPHERE_OBJECT_NAME);
    object_manager->RemoveObject(INFLOW_ZONE_OBJECT_NAME);
    object_manager->RemoveObject(OUTFLOW_ZONE_OBJECT_NAME);
  }



	void RSParticleSimulator::SetInflowDirection(const glm::vec3& direction_)
	{
		m_init_setting.m_inflow_direction = direction_;
		b_inflow_direction_changed = true;
	}

	void RSParticleSimulator::UpdateInflowDirection()
	{
		if (b_inflow_direction_changed)
		{

			const glm::vec3 normalized_inflow_direction =
        glm::normalize(m_init_setting.m_inflow_direction * glm::half_pi<float>());
			const auto rotation_quat = glm::quat(glm::vec3(0.0f, -normalized_inflow_direction.z, normalized_inflow_direction.y));
			m_inflow_transform = glm::mat4_cast(rotation_quat);

			b_inflow_direction_changed = false;
		}

		/*
		* glm::vec3 axisAngle = glm::vec3(x, y, z);
		* float angle = glm::length(axisAngle);         // Angle in radians
		* glm::vec3 axis = glm::normalize(axisAngle);   // Normalize to get rotation axis
		*
		* glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
		*
		*/
	}

}