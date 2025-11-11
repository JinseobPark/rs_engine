#include "pch.h"
#include "RSObjectManager.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/EventHandler/RSEventHandler.h"
#include "Resource/RSImguiHandler.h"

namespace RS_Object
{
	using RS_Event::mRSEVENTHANDLER;

	// ReSharper disable once CppMemberFunctionMayBeStatic
	void RSObjectManager::Initialize()
	{
		//Nothing
	}

	void RSObjectManager::Update(const float dt)
	{
		for (auto it = m_all_objects.begin(); it != m_all_objects.end(); ++it)
		{
			it->second->Update(dt);
		}
	}

	void RSObjectManager::Draw(RSShaderNames type_) const
	{
		// Not Yet without model
		for (auto& object : m_object_layers[static_cast<int>(type_)])
		{
			//object->Draw(type_);
		}
	}

	void RSObjectManager::DrawModel(RSRenderLayer type_, const RSShaderNames shader_, const bool is_opaque) const
	{
		for (const auto& object : m_object_layers[static_cast<int>(type_)])
		{
			const auto p_model = dynamic_cast<RSModel*>(object->GetComponent(RSComponentType::CT_MODEL));
			if (p_model == nullptr) continue;
			if (p_model->GetMesh() == nullptr) continue;
			if (p_model->GetIsVisible() == false) 
				continue;
			if (!p_model->GetIsOpaqueObject() == is_opaque)
				continue;
			if (shader_ != RSShaderNames::HUD_MAPPING)
			{
				RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "color", p_model->GetColor());
			}
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "model", object->GetTransformMatrix());
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "tex_matrix", p_model->GetTexTransform());

			if(!p_model->GetIsOpaqueObject())
				RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "opacity", p_model->GetTransparency());

			p_model->Draw();
		}

		// Draw models with other shader
	}

	void RSObjectManager::DrawWireFrameModel(RSRenderLayer type_, const RSShaderNames shader_) const
	{
		for (auto& object : m_object_layers[static_cast<int>(type_)])
		{
			const auto p_model = dynamic_cast<RSModel*>(object->GetComponent(RSComponentType::CT_MODEL));
			if (p_model == nullptr) continue;
			if (p_model->GetMesh() == nullptr) continue;
			if (p_model->GetIsVisible() == false)
				continue;
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "model", object->GetTransformMatrix());
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "color", glm::vec3(1.0f, 0.0f, 1.0f));
			p_model->DrawWireFrame();
		}

		// Draw models with other shader
	}

	void RSObjectManager::DrawHighlightModel(RSRenderLayer type_) const
	{
		constexpr auto current_shader = RSShaderNames::DEFAULT_MVPLT;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT("default"));

		for (auto& object : m_object_layers[static_cast<int>(type_)])
		{
			const auto p_model = dynamic_cast<RSModel*>(object->GetComponent(RSComponentType::CT_MODEL));
			if (p_model == nullptr) continue;
			if (p_model->GetMesh() == nullptr) continue;
			if (p_model->GetIsVisible() == false)
				continue;
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(current_shader, "model", object->GetTransformMatrix());
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(current_shader, "color", glm::vec3(0.0f, 0.0f, 0.0f));
			p_model->GetMesh()->GetBoundingBox()->DrawBoundingBox();
		}

	}

  void RSObjectManager::DrawPbrModel(RSRenderLayer type_, const RSShaderNames shader_, const bool is_opaque) const
  {
    for (auto& object : m_object_layers[static_cast<int>(type_)])
    {
      const auto p_model = dynamic_cast<RSModel*>(object->GetComponent(RSComponentType::CT_MODEL));
      if (p_model == nullptr) continue;
      if (p_model->GetMesh() == nullptr) continue;
      if (p_model->GetIsVisible() == false)
        continue;
      if (!p_model->GetMesh()->IsMaterialExist()) continue;

			if (!p_model->GetIsOpaqueObject() == is_opaque)
				continue;

      RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "model", object->GetTransformMatrix());
      RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "color", p_model->GetColor());
      RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "tex_matrix", p_model->GetTexTransform());

			if (!p_model->GetIsOpaqueObject())
				RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "opacity", p_model->GetTransparency());
			//RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "opacity", p_model->GetTransparency());
      RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "metallic",
                                                                    p_model->GetMesh()->GetMaterial()->GetMetallic());
      RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "roughness",
                                                                    p_model->GetMesh()->GetMaterial()->GetRoughness());
      RSResourceManager::GetInstance()->GetShaderManager()->SetData(shader_, "ambient",
                                                                    p_model->GetMesh()->GetMaterial()->GetAmbient());
			p_model->DrawPbr();
    }
  }

  void RSObjectManager::Shutdown()
	{
    // null pointer to m_last_picked_object
		m_last_picked_object = nullptr;

		// Remove m_collision_objects
    m_collision_objects.clear();

		// Remove picked objects
    m_picked_object_list.clear();

		// Remove object layers
    for (auto& m_object_layer : m_object_layers)
    {
      m_object_layer.clear();
    }

    // Remove all objects
		for (auto& m_all_object : m_all_objects)
		{
			delete m_all_object.second;
		}
		m_all_objects.clear();
	}

	bool RSObjectManager::AddObject(RSObject* p_object)
	{
		if (m_all_objects.contains(p_object->GetName()))
		{
			RS_WARN("RSObject already exists");
			return false;
		}
		m_all_objects[p_object->GetName()] = p_object;
		m_object_layers[static_cast<int>(p_object->GetRenderLayer())].push_back(p_object);
		return true;
	}

	RSObject* RSObjectManager::CreateObject(const std::string& name_)
	{
		if (m_all_objects.contains(name_))
		{
			RS_WARN("RSObject already exists");
      // return the existing object
      return m_all_objects[name_];
		}
		const auto pObject = new RSObject();
		pObject->SetName(name_);
		m_all_objects[name_] = pObject;
		m_object_layers[static_cast<int>(pObject->GetRenderLayer())].push_back(pObject);

		return pObject;
	}

	bool RSObjectManager::RemoveObject(RSObject* p_object)
	{
		// Remove from target layer
		m_object_layers[static_cast<int>(p_object->GetRenderLayer())].erase(std::ranges::remove(m_object_layers[static_cast<int>(p_object->GetRenderLayer())], p_object).begin(), m_object_layers[static_cast<int>(p_object->GetRenderLayer())].end());
		// find the object exist in highlight object list
		if (const auto its = std::ranges::find(m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)], p_object); its != m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].end())
		{
			m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].erase(its);
		}

		// If m_collision_objects has the object, remove it
		if (const auto it = std::ranges::find(m_collision_objects, p_object); it != m_collision_objects.end())
		{
			m_collision_objects.erase(it);
		}

		if (RS_Handler::mRSIMGUIHANDLER->GetSelectedObject() == p_object)
			RS_Handler::mRSIMGUIHANDLER->ResetSelectedObject();
		if (const auto it = m_all_objects.find(p_object->GetName()); it != m_all_objects.end())
		{
			delete it->second;
			m_all_objects.erase(it);
			return true;
		}
		RS_WARN("RSObject not found");
		return false;
	}

	bool RSObjectManager::RemoveObject(const std::string& name_)
	{
		if (m_all_objects.contains(name_))
      return RemoveObject(m_all_objects[name_]);

		return false;
	}

	RSObject* RSObjectManager::GetObject(const std::string& name_)
	{
		if (const auto it = m_all_objects.find(name_); it != m_all_objects.end())
		{
			return it->second;
		}
		RS_INFO("%s RSObject not found", name_.c_str());
		return nullptr;
	}

	RSObject* RSObjectManager::FindObject(const std::string& name_)
	{
		if (const auto it = m_all_objects.find(name_); it != m_all_objects.end())
		{
			return it->second;
		}
		return nullptr;
	}
	bool RSObjectManager::SelectObjectByMousePicking(int x_, int y_, bool is_ctrl)
	{
		// JUST FOR OBJECT SELECTING
		// Get Camera Data
		glm::mat4 mat_view = RSResourceManager::GetInstance()->GetCamera()->GetView();
		glm::mat4 mat_proj = RSResourceManager::GetInstance()->GetCamera()->GetProj();
		const glm::vec3 camera_position = RSResourceManager::GetInstance()->GetCamera()->GetPosition();
		const unsigned int WIDTH = RS_Application::mRSAPPLICATION->GetWidth();
		const unsigned int HEIGHT = RS_Application::mRSAPPLICATION->GetHeight();

		// Set Ray
		auto origin = glm::vec4(), dir = glm::vec4();

		double vx = (2.0 * x_ / WIDTH - 1.0);
		double vy = -(-2.0 * -y_ / HEIGHT - 1.0);
		double vz = -1.0;

		auto inverse_view = glm::mat4(1.0f);
		// Assume perspective camera

		if (RSResourceManager::GetInstance()->GetCamera()->GetCameraIsPerspective())
		{
			inverse_view = glm::inverse(mat_view);
			vx /= mat_proj[0][0];
			vy /= mat_proj[1][1];
			origin = inverse_view * glm::vec4(0, 0, 0, 1);
			dir = inverse_view * glm::vec4(vx, vy, vz, 0);
			dir = glm::normalize(dir);
		}

		//When the camera is orthographic
		else
		{
			inverse_view = glm::inverse(mat_view) * glm::inverse(mat_proj);
			origin = inverse_view * glm::vec4(vx, vy, vz, 1);
			dir = glm::inverse(mat_view) * glm::vec4(0, 0, -1, 0);
			dir = glm::normalize(dir);
		}
		
		if (origin.w != 1)
		{
			origin.x /= origin.w;
			origin.y /= origin.w;
			origin.z /= origin.w;
		}

		// Ray Casting

		// how to check collision?
		// bounding box vs vertex ray casting

		// Clear highlight object list
		if (is_ctrl == false)
		{
			m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].clear();
			m_picked_object_list.clear();
		}

		// Set Check object list
		std::vector<RSObject*> check_objects_list;
		// check object layer : R_SURFACE_OPAQUE, R_LINE_OPAQUE, R_POINT, R_SURFACE_WIREFRAME_OPAQUE, R_SURFACE_TRANSPARENT, R_LINE_TRANSPARENT, R_SURFACE_WIRE_TRANSPARENT
		check_objects_list.reserve(
			m_object_layers[static_cast<int>(RSRenderLayer::R_SURFACE_OPAQUE)].size() +
			m_object_layers[static_cast<int>(RSRenderLayer::R_LINE_OPAQUE)].size() +
			m_object_layers[static_cast<int>(RSRenderLayer::R_POINT)].size() +
			m_object_layers[static_cast<int>(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE)].size()
			//m_object_layers[(int)RSRenderLayer::R_SURFACE_TRANSPARENT].size() +
			//m_object_layers[(int)RSRenderLayer::R_LINE_TRANSPARENT].size() +
			//m_object_layers[(int)RSRenderLayer::R_SURFACE_WIRE_TRANSPARENT].size()
		);
		check_objects_list.insert(check_objects_list.end(), m_object_layers[static_cast<int>(RSRenderLayer::R_SURFACE_OPAQUE)].begin(), m_object_layers[static_cast<int>(RSRenderLayer::R_SURFACE_OPAQUE)].end());
		check_objects_list.insert(check_objects_list.end(), m_object_layers[static_cast<int>(RSRenderLayer::R_LINE_OPAQUE)].begin(), m_object_layers[static_cast<int>(RSRenderLayer::R_LINE_OPAQUE)].end());
		check_objects_list.insert(check_objects_list.end(), m_object_layers[static_cast<int>(RSRenderLayer::R_POINT)].begin(), m_object_layers[static_cast<int>(RSRenderLayer::R_POINT)].end());
		check_objects_list.insert(check_objects_list.end(), m_object_layers[static_cast<int>(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE)].begin(), m_object_layers[static_cast<int>(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE)].end());
		//check_objects_list.insert(check_objects_list.end(), m_object_layers[(int)RSRenderLayer::R_SURFACE_TRANSPARENT].begin(), m_object_layers[(int)RSRenderLayer::R_SURFACE_TRANSPARENT].end());
		//check_objects_list.insert(check_objects_list.end(), m_object_layers[(int)RSRenderLayer::R_LINE_TRANSPARENT].begin(), m_object_layers[(int)RSRenderLayer::R_LINE_TRANSPARENT].end());
		//check_objects_list.insert(check_objects_list.end(), m_object_layers[(int)RSRenderLayer::R_SURFACE_WIRE_TRANSPARENT].begin(), m_object_layers[(int)RSRenderLayer::R_SURFACE_WIRE_TRANSPARENT].end());
		
		std::list<RSObject*> passed_bounding_box_objects;
		RSObject* result_target_object = nullptr;
		// Check Collision
		for (const auto& target_object : check_objects_list)
		{
			auto p_model = dynamic_cast<RSModel*>(target_object->GetComponent(RSComponentType::CT_MODEL));
			if (p_model == nullptr)
				continue;
			if(!p_model->GetIsVisible())
				continue;
			if (p_model->GetMesh() == nullptr)
				continue;

			// Get bounding box
			RS_Mesh::RSBoundingBox* p_bounding_box = p_model->GetMesh()->GetBoundingBox();
			if (p_bounding_box == nullptr)
				continue;

			// Get inverse transform matrix
			glm::mat4 inverse_transform = glm::inverse(target_object->GetTransformMatrix());
			glm::vec4 origin_local = inverse_transform * origin;
			glm::vec4 dir_local = inverse_transform * dir;
			dir_local = glm::normalize(dir_local);

			// Check Collision
			if (p_bounding_box->IsCheckCollisionRayCast(glm::vec3(origin_local), glm::vec3(dir_local)))
			{
				passed_bounding_box_objects.push_back(target_object);
			}
		}

		// Find the closest object
		float min_distance = FLT_MAX;
		for (const auto& target_object : passed_bounding_box_objects)
		{
			auto p_model = dynamic_cast<RSModel*>(target_object->GetComponent(RSComponentType::CT_MODEL));
			if (p_model == nullptr)
				continue;
			if (p_model->GetMesh() == nullptr)
				continue;

			float determinant = glm::determinant(target_object->GetTransformMatrix());
			// Get inverse transform matrix
			glm::mat4 inverse_transform = glm::inverse(target_object->GetTransformMatrix());
			glm::vec4 origin_local = inverse_transform * origin;
			glm::vec4 dir_local = inverse_transform * dir;
			dir_local = glm::normalize(dir_local);

			glm::vec3 local_hit_position;
			glm::vec3 world_hit_position;

			p_model->GetMesh()->GetRayDistance(glm::vec3(origin_local), glm::vec3(dir_local), local_hit_position);
			world_hit_position = glm::vec3(target_object->GetTransformMatrix() * glm::vec4(local_hit_position, 1.0f));
			float world_object_distance = glm::length(world_hit_position - camera_position);

			if (world_object_distance < min_distance)
			{
				min_distance = world_object_distance;
				result_target_object = target_object;
			}
		}

		// finally, add or remove object to highlight object list with control.
		if (result_target_object != nullptr)
		{
			m_last_picked_object = result_target_object;
			if (is_ctrl == true)
			{
				// Layer check, is already in highlight object list
				if (auto its = std::ranges::find(m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)], m_last_picked_object); its != m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].end())
				{
					m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].erase(its);

					if (auto it = std::ranges::find(m_picked_object_list, m_last_picked_object); it != m_picked_object_list.end())
					{
						m_picked_object_list.erase(it);
					}
				}
				else // Add 
				{
					m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].push_back(m_last_picked_object);
					m_picked_object_list.push_back(m_last_picked_object);
					mRSEVENTHANDLER->GetImguiHandler()->SelectedObjectByMouse(m_last_picked_object);
				}
				return true;
			}
			else
			{
				m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].push_back(m_last_picked_object);
				m_picked_object_list.push_back(m_last_picked_object);
				mRSEVENTHANDLER->GetImguiHandler()->SelectedObjectByMouse(m_last_picked_object);
				RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetTargetObject(m_last_picked_object);
				RSResourceManager::GetInstance()->GetCamera()->SetRotateAxis(m_last_picked_object->GetTransform()->GetPosition());
				return true;
			}
		}
		m_last_picked_object = nullptr;
		mRSEVENTHANDLER->GetImguiHandler()->SelectedObjectByMouse(nullptr);
		RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetTargetObject(nullptr);
		RSResourceManager::GetInstance()->GetCamera()->SetRotateAxis(glm::vec3(0.0f));
		return false;
		
	}

	void RSObjectManager::ReleasePicking() const
	{
		if (m_last_picked_object == nullptr)
			RSResourceManager::GetInstance()->GetCamera()->SetRotateAxis(glm::vec3(0.0f));
		else
			RSResourceManager::GetInstance()->GetCamera()->SetRotateAxis(m_last_picked_object->GetTransform()->GetPosition());
		

	}
	void RSObjectManager::ClearPickingObjectList()
	{
		m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].clear();
		m_picked_object_list.clear();
		m_last_picked_object = nullptr;
	}

	void RSObjectManager::SetPickingObject(RSObject* p_object)
	{
		if (m_last_picked_object == p_object)
			return;
    m_last_picked_object = p_object;
		m_object_layers[static_cast<int>(RSRenderLayer::R_HIGHLIGHT)].push_back(m_last_picked_object);
		m_picked_object_list.push_back(m_last_picked_object);
		RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetTargetObject(m_last_picked_object);
		RSResourceManager::GetInstance()->GetCamera()->SetRotateAxis(m_last_picked_object->GetTransform()->GetPosition());
	}

	void RSObjectManager::CalculateTargetSizeData()
	{
		bool is_exist_object = false;


		// Initialize target size data
		m_target_size_data[0] = m_target_size_data[2] = m_target_size_data[4] = FLT_MAX; // x, y, z
		m_target_size_data[1] = m_target_size_data[3] = m_target_size_data[5] = -FLT_MAX; // x, y, z


		// Calculate about all object min/max world position
		for (const auto& target_object : m_all_objects)
		{
			RSObject* p_object = target_object.second;
			const auto p_model = static_cast<RSModel*>(p_object->GetComponent(RSComponentType::CT_MODEL));
			if (p_model == nullptr)
				continue;
			if (p_model->GetMesh() == nullptr)
				continue;

			is_exist_object = true;
			// Get inverse transform matrix
			glm::mat4 inverse_transform = glm::inverse(p_object->GetTransformMatrix());
			glm::vec3 min_local = p_model->GetMesh()->GetBoundingBox()->GetMin();
			glm::vec3 max_local = p_model->GetMesh()->GetBoundingBox()->GetMax();

			// Get world position
			const auto min_world = glm::vec3(p_object->GetTransformMatrix() * glm::vec4(min_local, 1.0f));
			const auto max_world = glm::vec3(p_object->GetTransformMatrix() * glm::vec4(max_local, 1.0f));


			// Check min/max
			m_target_size_data[0] = glm::min(min_world.x, m_target_size_data[0]);
			m_target_size_data[2] = glm::min(min_world.y, m_target_size_data[2]);
			m_target_size_data[4] = glm::min(min_world.z, m_target_size_data[4]);

			m_target_size_data[1] = glm::max(max_world.x, m_target_size_data[1]);
			m_target_size_data[3] = glm::max(max_world.y, m_target_size_data[3]);
			m_target_size_data[5] = glm::max(max_world.z, m_target_size_data[5]);

		}

		// When there is no mesh object, set default value
		if (!is_exist_object)
		{
			// Set default value size 1.
			m_target_size_data[0] = m_target_size_data[2] = m_target_size_data[4] = -1.0f; // x, y, z
			m_target_size_data[1] = m_target_size_data[3] = m_target_size_data[5] = 1.0f; // x, y, z
		}
	}

	std::array<float, 6>& RSObjectManager::GetTargetSizeData()
	{
		CalculateTargetSizeData();

		return m_target_size_data;
	}
	void RSObjectManager::CalledRemoveObject()
	{
		if (m_picked_object_list.empty())
			return;

		for (auto& object : m_picked_object_list)
		{
			RemoveObject(object);
		}
		m_picked_object_list.clear();
		m_last_picked_object = nullptr;
	}

	RSObject* RSObjectManager::CreateCubeObject(const std::string& name_)
	{
		std::string cube_name;
		if (name_.empty())
		{
		  // Find empty name from 'Cube_0' ~ 'Cube_n'. n is the number of cubes
		  cube_name = "Cube_";
		  int cube_number = 0;
		  while (m_all_objects.contains(cube_name + std::to_string(cube_number)))
		  {
			  cube_number++;
		  }
		  cube_name += std::to_string(cube_number);
    }
    else
    {
      cube_name = name_;
    }

		// Create Cube Object
		const auto p_object = new RSObject();
		p_object->SetName(cube_name);
		const auto p_model = new RSModel();
		p_object->AddComponent(p_model);
		const auto p_mesh = new RSMesh();
		p_mesh->SetMeshData(RSResourceManager::GetInstance()->GetMeshManager()->GetMesh("Cube"));
		p_mesh->SetIsTexture(true);

		p_model->SetMesh(p_mesh);

		AddObject(p_object);
		m_collision_objects.push_back(p_object);

		return p_object;
	}
	RSObject* RSObjectManager::CreateSphereObject(const std::string& name_)
	{
    std::string sphere_name;
    if (name_.empty())
    {
      // Find empty name from 'Sphere_0' ~ 'Sphere_n'. n is the number of spheres
      sphere_name = "Sphere_";
      int sphere_number = 0;
      while (m_all_objects.contains(sphere_name + std::to_string(sphere_number)))
      {
        sphere_number++;
      }
      sphere_name += std::to_string(sphere_number);
    }
    else
    {
      sphere_name = name_;
    }
    // Create Sphere Object
    const auto p_object = new RSObject();
    p_object->SetName(sphere_name);
    const auto p_model = new RSModel();
    p_object->AddComponent(p_model);
    const auto p_mesh = new RSMesh();
    p_mesh->SetMeshData(RSResourceManager::GetInstance()->GetMeshManager()->GetMesh("Sphere"));
    p_mesh->SetIsTexture(true);
    p_model->SetMesh(p_mesh);
    AddObject(p_object);
    m_collision_objects.push_back(p_object);
		return p_object;
	}

	RSObject* RSObjectManager::CreatePlaneObject(const std::string& name_)
	{
    std::string plane_name;
    if (name_.empty())
    {
      // Find empty name from 'Plane_0' ~ 'Plane_n'. n is the number of planes
      plane_name = "Plane_";
      int plane_number = 0;
      while (m_all_objects.contains(plane_name + std::to_string(plane_number)))
      {
        plane_number++;
      }
      plane_name += std::to_string(plane_number);
    }
    else
    {
      plane_name = name_;
    }
    // Create Plane Object
    const auto p_object = new RSObject();
    p_object->SetName(plane_name);
    const auto p_model = new RSModel();
    p_object->AddComponent(p_model);
    const auto p_mesh = new RSMesh();
    p_mesh->SetMeshData(RSResourceManager::GetInstance()->GetMeshManager()->GetMesh("Plane"));
    p_mesh->SetIsTexture(true);
    p_model->SetMesh(p_mesh);
    AddObject(p_object);
    m_collision_objects.push_back(p_object);
		return p_object;
	}

	void RSObjectManager::RemoveObjectsLevelChanged()
	{
		mRSEVENTHANDLER->GetImguiHandler()->ResetSelectedObject();
		// null pointer to m_last_picked_object
		m_last_picked_object = nullptr;

		// Remove m_collision_objects
		m_collision_objects.clear();

		// Remove picked objects
		m_picked_object_list.clear();

		// Remove object layers
		for (auto& m_object_layer : m_object_layers)
		{
			m_object_layer.clear();
		}

		// Remove all objects
		for (auto& m_all_object : m_all_objects)
		{
			delete m_all_object.second;
		}
		m_all_objects.clear();
	}

	void RSObjectManager::SetCopyObjectsToName(const std::list<RSObject*>& copy_objects_)
	{
    m_copy_objects_to_name.clear();
    for (const auto& object : copy_objects_)
    {
      m_copy_objects_to_name.push_back(object->GetName());
    }
	}

	void RSObjectManager::PasteTheCopiedObjects()
	{
    // If there is no copied object, return
    if (m_copy_objects_to_name.empty())
      return;

    // Clear the picked object list
		ClearPickingObjectList();

		// Loop
    for (const auto& object_name : m_copy_objects_to_name)
    {
      // Find the object with the name
      if (const auto it = m_all_objects.find(object_name); it != m_all_objects.end())
      {
        // Copy the object
        const auto p_new_object = CopyObject(it->second);
        // Set the new object to the picked object list
        SetPickingObject(p_new_object);
				mRSEVENTHANDLER->GetImguiHandler()->SelectedObjectByMouse(p_new_object);
			}
    }
	}

	RSObject* RSObjectManager::CopyObject(const RSObject* p_object)
	{
    const auto p_new_object = new RSObject(*p_object);

    // Get the name of p_object for the new object's name _number.
    // If the name is "obj_name", the new object's name is "obj_name_1"
    // If the name is "obj_name_1", the new object's name is "obj_name_2"

    std::string new_object_name = p_object->GetName();
		if (const auto pos = new_object_name.find_last_of('_'); pos != std::string::npos)
    {
      const std::string number_string = new_object_name.substr(pos + 1);
      // If the number_string is not a number, the new object name is "obj_name_1"
      if (!std::ranges::all_of(number_string, ::isdigit))
      {
        new_object_name += "_1";
      }
      else
      {
        new_object_name = new_object_name.substr(0, pos);
        int number = std::stoi(number_string);
				while (m_all_objects.contains(new_object_name + "_" + std::to_string(number)))
				{
					number++;
				}
        new_object_name += "_" + std::to_string(number);
      }
    }
    else
    {
			int number = 1;
			while (m_all_objects.contains(new_object_name + "_" + std::to_string(number)))
			{
				number++;
			}
			new_object_name += "_" + std::to_string(number);
    }

		// Get last number after last '_' + number from name


   // // Find "new_object_name += "_1"" named object first. If it exists, increase the number.
   // if (m_all_objects.contains(new_object_name + "_1"))
   // {
   //   // Find the last number of the object list.
   //   int number = 1;
   //   while (m_all_objects.contains(new_object_name + "_" + std::to_string(number)))
   //   {
   //     number++;
   //   }
   //   new_object_name += "_" + std::to_string(number);
   // }
   // else
   // {
			//// object_name
   //   new_object_name += "_1";
   // }



		//// Get last number after last '_' + number
  //  if (const auto pos = new_object_name.find_last_of('_'); pos != std::string::npos)
  //  {
  //    const std::string number_string = new_object_name.substr(pos + 1);
  //    // If the number_string is not a number, the new object name is "obj_name_1"
  //    if (!std::ranges::all_of(number_string, ::isdigit))
  //    {
  //      new_object_name += "_1";
  //    }
  //    else
  //    {
  //      new_object_name = new_object_name.substr(0, pos);
  //      int number = std::stoi(number_string);
  //      number++;
  //      new_object_name += "_" + std::to_string(number);
  //    }
  //  }
  //  else
  //  {
  //    new_object_name += "_1";
  //  }

    // Set the new object's name
    p_new_object->SetName(new_object_name);

    // Add the new object to the object manager
    AddObject(p_new_object);
		m_collision_objects.push_back(p_new_object);

		return p_new_object;
	}

	void RSObjectManager::DuplicateObjects()
	{
		// copy
    const std::list <RSObject*> copy_picked_obj_list = m_picked_object_list;

		// Clear the picked object list
		ClearPickingObjectList();

		// Loop
		for (const auto& object : copy_picked_obj_list)
		{
			// Find the object with the name
			if (const auto it = m_all_objects.find(object->GetName()); it != m_all_objects.end())
			{
				// Copy the object
				const auto p_new_object = CopyObject(it->second);
				// Set the new object to the picked object list
				SetPickingObject(p_new_object);
				m_collision_objects.push_back(p_new_object);
				mRSEVENTHANDLER->GetImguiHandler()->SelectedObjectByMouse(p_new_object);
			}
		}
	}
	void RSObjectManager::SelectAllObjects()
	{
    // clear the picked object list first.
		ClearPickingObjectList();

    // Loop
    for (const auto& object : m_all_objects)
    {
      // Set the object to the picked object list
      SetPickingObject(object.second);
			mRSEVENTHANDLER->GetImguiHandler()->SelectedObjectByMouse(object.second);
		}

	}
}
