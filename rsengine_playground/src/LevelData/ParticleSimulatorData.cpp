#include "pch.h"
#include "ParticleSimulatorData.h"
#include "Util/RSUtilFunctions.h"
#include "GameData.h"

#define USE_PLAYER true

namespace SJG
{
	ParticleSimulatorData::ParticleSimulatorData()
	{
		m_player = nullptr;
		m_legend_widget = nullptr;
		m_resource_manager = RSResourceManager::GetInstance();
	}

	ParticleSimulatorData::~ParticleSimulatorData()
	{
	}

	void ParticleSimulatorData::Initialize()
	{
		m_resource_manager->GetGraphicsData()->clear_color[0] = 0.19921875f;
		m_resource_manager->GetGraphicsData()->clear_color[1] = 0.19921875f;
		m_resource_manager->GetGraphicsData()->clear_color[2] = 0.19921875f;
#if USE_PLAYER
		InitializePlayer();
#endif
		{ // Grid object
			// Plane RSObject
			auto plane = m_resource_manager->GetObjectManager()->CreateObject("Grid");

			RS_Component::RSModel* plane_model = new RS_Component::RSModel();
			plane->AddComponent(plane_model);

			RS_Mesh::RSMesh* plane_mesh = new RS_Mesh::RSMesh();
			plane_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Grid"));
			plane_mesh->SetIsTexture(true);

			plane_model->SetMesh(plane_mesh);
			plane->GetTransform()->SetScale(glm::vec3(30.0f, 10.0f, 10.0f));
		}



		{ // Cube Object1
			auto cube = m_resource_manager->GetObjectManager()->CreateCubeObject("CollisionCube1");

			cube->GetTransform()->SetPosition(glm::vec3(0.0f, 5.0f, -5.5f));
			cube->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
			cube->GetTransform()->SetScale(glm::vec3(30.0f, 10.0f, 1.0f));
      m_object_list.push(cube);
		}
		{ // Cube Object2
			auto cube = m_resource_manager->GetObjectManager()->CreateCubeObject("CollisionCube2");

			cube->GetTransform()->SetPosition(glm::vec3(-15.5f, 5.0f, -0.5f));
			cube->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
			cube->GetTransform()->SetScale(glm::vec3(1.0f, 10.0f, 11.0f));
			m_object_list.push(cube);
		}
		{ // Cube Object3
			auto cube = m_resource_manager->GetObjectManager()->CreateCubeObject("CollisionCube3");

			cube->GetTransform()->SetPosition(glm::vec3(15.5f, 5.0f, -0.5f));
			cube->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
			cube->GetTransform()->SetScale(glm::vec3(1.0f, 10.0f, 11.0f));
			m_object_list.push(cube);
		}

		//{ // Collision ticle 1
		//	m_resource_manager->GetObjectManager()->CreateObject("CollisionObj1");
		//	auto collision_obj1 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj1");

		//	RS_Component::RSModel* collision_model1 = new RS_Component::RSModel();
		//	collision_obj1->AddComponent(collision_model1);

		//	RS_Mesh::RSMesh* collision_mesh1 = new RS_Mesh::RSMesh();
		//	collision_mesh1->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("tetrapod"));
		//	collision_mesh1->SetIsTexture(true);

		//	collision_model1->SetMesh(collision_mesh1);
		//	collision_obj1->GetTransform()->SetPosition(glm::vec3(4.3f, 2.0f, -4.5f));
		//	collision_obj1->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 90.0f));
		//	collision_obj1->GetTransform()->SetScale(2.0f);
		//}

		//{ // Collision ticle 2
		//	m_resource_manager->GetObjectManager()->CreateObject("CollisionObj2");
		//	auto collision_obj2 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj2");

		//	RS_Component::RSModel* collision_model2 = new RS_Component::RSModel();
		//	collision_obj2->AddComponent(collision_model2);

		//	RS_Mesh::RSMesh* collision_mesh2 = new RS_Mesh::RSMesh();
		//	collision_mesh2->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("tetrapod"));
		//	collision_mesh2->SetIsTexture(true);

		//	collision_model2->SetMesh(collision_mesh2);
		//	collision_obj2->GetTransform()->SetPosition(glm::vec3(4.45f, 2.0f, -0.727f));
		//	collision_obj2->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 50.0f));
		//	collision_obj2->GetTransform()->SetScale(2.0f);
		//}

		//{ // Collision ticle 3
		//	m_resource_manager->GetObjectManager()->CreateObject("CollisionObj3");
		//	auto collision_obj3 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj3");

		//	RS_Component::RSModel* collision_model3 = new RS_Component::RSModel();
		//	collision_obj3->AddComponent(collision_model3);

		//	RS_Mesh::RSMesh* collision_mesh3 = new RS_Mesh::RSMesh();
		//	collision_mesh3->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("tetrapod"));
		//	collision_mesh3->SetIsTexture(true);

		//	collision_model3->SetMesh(collision_mesh3);
		//	collision_obj3->GetTransform()->SetPosition(glm::vec3(4.4f, 2.0f, 3.6f));
		//	collision_obj3->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 210.0f));
		//	collision_obj3->GetTransform()->SetScale(2.0f);
		//}

		//{ // Collision ticle 4
		//	m_resource_manager->GetObjectManager()->CreateObject("CollisionObj4");
		//	auto collision_obj4 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj4");

		//	RS_Component::RSModel* collision_model4 = new RS_Component::RSModel();
		//	collision_obj4->AddComponent(collision_model4);

		//	RS_Mesh::RSMesh* collision_mesh4 = new RS_Mesh::RSMesh();
		//	collision_mesh4->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("tetrapod"));
		//	collision_mesh4->SetIsTexture(true);

		//	collision_model4->SetMesh(collision_mesh4);
		//	collision_obj4->GetTransform()->SetPosition(glm::vec3(3.365f, 2.0f, 8.16f));
		//	collision_obj4->GetTransform()->SetRotation(glm::vec3(53.25f, 72.6f, 74.5f));
		//	collision_obj4->GetTransform()->SetScale(2.0f);
		//}

		//{ // Collision ticle 5
		//	m_resource_manager->GetObjectManager()->CreateObject("CollisionObj5");
		//	auto collision_obj5 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj5");

		//	RS_Component::RSModel* collision_model5 = new RS_Component::RSModel();
		//	collision_obj5->AddComponent(collision_model5);

		//	RS_Mesh::RSMesh* collision_mesh5 = new RS_Mesh::RSMesh();
		//	collision_mesh5->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("tetrapod"));
		//	collision_mesh5->SetIsTexture(true);

		//	collision_model5->SetMesh(collision_mesh5);
		//	collision_obj5->GetTransform()->SetPosition(glm::vec3(5.245f, 2.0f, -7.979f));
		//	collision_obj5->GetTransform()->SetRotation(glm::vec3(53.25f, 72.6f, 74.5f));
		//	collision_obj5->GetTransform()->SetScale(2.0f);
		//}

		if (m_legend_widget == nullptr)
		{
			m_legend_widget = new RS_Widget::RSLegendWidget();
			m_legend_widget->Initialize();
			// The Legend widget is clickable. When dragging the legend widget, the legend widget is moved.

			m_legend_widget->SetPosition(0.9f, 0.75f);
			m_legend_widget->SetSize(20.0f, 120.0f);
			m_legend_widget->SetVertical(true);
			m_legend_widget->SetWidgetName(LEGEND_WIDGET_NAME);
			m_resource_manager->GetWidgetManager()->AddButtonWidgetID(m_legend_widget, 60);


		}

	}

	void ParticleSimulatorData::InitializePlayer()
	{
		m_player = m_resource_manager->GetObjectManager()->CreateObject("Player");

		RS_ASSERT(m_player != nullptr ,"Failed to create the player");

		// if player does not have model component, create one and add this
		if (!m_player->GetComponent(RSComponentType::CT_MODEL))
		{

			RS_Component::RSModel* player_model = new RS_Component::RSModel();
			m_player->AddComponent(player_model);

			RS_Mesh::RSMesh* player_mesh = new RS_Mesh::RSMesh();
			player_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Sphere"));
			player_mesh->SetTextureID(m_resource_manager->GetTextureManager()->GetTexture("default")->GetTexture());
			player_mesh->SetIsTexture(true);

			player_model->SetMesh(player_mesh);
			
		}

		if (!m_player->GetComponent(RSComponentType::CT_RIGIDBODY))
		{
			RS_Component::RSRigidbody* player_rigidbody = new RS_Component::RSRigidbody();
			m_player->AddComponent(player_rigidbody);
		}

		m_player->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
		m_player->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		m_player->GetTransform()->SetScale(2.0f);

		b_player_on_ground = true;
		m_player_speed = 5.0f;
	}


	void ParticleSimulatorData::Update(const float dt)
	{
#if USE_PLAYER
		// Lets Play the game
		m_player = m_resource_manager->GetObjectManager()->GetObject("Player");
		if (m_player == nullptr)
			return;

		RS_Component::RSRigidbody* player_rigidbody = static_cast<RS_Component::RSRigidbody*>(m_player->GetComponent(RSComponentType::CT_RIGIDBODY));
		RS_ASSERT(player_rigidbody != nullptr, "Failed to find the rigidbody component from the player");
		// when player on the ground, player's rigidbody should be zero
		if (m_player->GetTransform()->GetPosition().y <= 1.0f)
		{
			m_player->GetTransform()->SetPosition(glm::vec3(m_player->GetTransform()->GetPosition().x, 1.0f, m_player->GetTransform()->GetPosition().z));
			player_rigidbody->SetVelocity(glm::vec3(0.0f));
			player_rigidbody->SetGravity(30.0f);
			b_player_on_ground = true;
		}

		float distance = m_player_speed * dt;
		glm::vec3 player_direction = glm::vec3(0.0f);
		glm::vec3 rotation_axis = glm::vec3(0.0f);

		if (is_auto_play)
		{
			auto_jump_time -= dt;
			if (auto_jump_time < 0.0f)
			{
				if (b_player_on_ground)
				{
					player_rigidbody->SetVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
					b_player_on_ground = false;
				}
				auto_jump_time = 2.0f;
			}
			// when player is out of ground (-9.0f, 9.0f), player direction should be changed to random x,z axis
			glm::vec3 player_position_temp = m_player->GetTransform()->GetPosition();
			bool is_out_of_ground = false;

			if (m_player->GetTransform()->GetPosition().x < -6.0f)
			{
				player_position_temp.x = -6.0f;
				is_out_of_ground = true;
			}
			else if (m_player->GetTransform()->GetPosition().x > 14.0f)
			{
				player_position_temp.x = 14.0f;
				is_out_of_ground = true;
			}
			else if (m_player->GetTransform()->GetPosition().z < -4.0f)
			{
				player_position_temp.z = -4.0f;
				is_out_of_ground = true;
			}
			else if (m_player->GetTransform()->GetPosition().z > 4.0f)
			{
				player_position_temp.z = 4.0f;
				is_out_of_ground = true;
			}
			else
			{
				player_direction = auto_player_direction * distance;
				m_player->GetTransform()->AddPosition(player_direction);
				rotation_axis = glm::vec3(auto_player_direction.z, 0.0f, -auto_player_direction.x);
				float rotation_angle = glm::length(player_direction);
				glm::quat rotation = glm::angleAxis(rotation_angle, rotation_axis);
				m_player->GetTransform()->AddRotation(rotation);
			}

			if (is_out_of_ground)
			{
				m_player->GetTransform()->SetPosition(player_position_temp);
				// New Random direction
				float random_x = RS_Util::GetRandomNumberExcludeZero();
				float random_z = RS_Util::GetRandomNumberExcludeZero();
				auto_player_direction = normalize(glm::vec3(random_x, 0.0f, random_z));
			}
		}
#endif
		KeyUpdate(dt);

	}

	void ParticleSimulatorData::KeyUpdate(float dt)
	{
#if USE_PLAYER

		if (!is_auto_play)
		{
			_ASSERTE(m_player != nullptr && "Failed to find the player");
			RS_Component::RSRigidbody* player_rigidbody = static_cast<RS_Component::RSRigidbody*>(m_player->GetComponent(RSComponentType::CT_RIGIDBODY));
			_ASSERTE(player_rigidbody != nullptr && "Failed to find the rigidbody component from the player");

			// Player control with "WASD" key to move X,Z axis
			glm::vec3 player_direction = glm::vec3(0.0f);
			glm::vec3 rotation_axis = glm::vec3(0.0f);
			float distance = m_player_speed * dt;

			if (RS_Input::mRSINPUT->IsPressed(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAYER_FORWARD)))
			{
				player_direction.z = -1.0f;
				rotation_axis.x = -1.0f;
			}
			if (RS_Input::mRSINPUT->IsPressed(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAYER_BACKWARD)))
			{
				player_direction.z = 1.0f;
				rotation_axis.x = 1.0f;
			}
			if (RS_Input::mRSINPUT->IsPressed(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAYER_LEFT)))
			{
				player_direction.x = -1.0f;
				rotation_axis.z = 1.0f;
			}
			if (RS_Input::mRSINPUT->IsPressed(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAYER_RIGHT)))
			{
				player_direction.x = 1.0f;
				rotation_axis.z = -1.0f;
			}
			if (RS_Input::mRSINPUT->IsPressed(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAYER_JUMP)))
			{
				if (b_player_on_ground)
				{
					player_rigidbody->SetVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
					b_player_on_ground = false;
				}
			}

			if (glm::length(player_direction) != 0.0f)
			{
				player_direction = glm::normalize(player_direction);
				player_direction *= distance;
				m_player->GetTransform()->AddPosition(player_direction);
				// Calculate the rotation angle
				// Formula : l = r * theta ,r = 1, l = player_direction
				float rotation_angle = glm::length(player_direction);//*2.0f / glm::pi<float>();
				glm::vec3 normalized_axis = glm::normalize(rotation_axis);
				if (glm::length(normalized_axis) != 0.0f)
				{
					glm::quat rotation = glm::angleAxis(rotation_angle, normalized_axis);
					m_player->GetTransform()->AddRotation(rotation);
				}
			}
		}

		if (RS_Input::mRSINPUT->IsTriggered(RSKey::INSERT))
		{
			is_auto_play = !is_auto_play;
		}
#endif

		// Test Code
		if (RS_Input::mRSINPUT->IsTriggered(RSKey::B))
		{
			m_legend_widget->SetDividedValue(0);
			//RSResourceManager::GetInstance()->GetParticleManager()->GetVTKViewer()->SetLegendTextureId(m_legend_widget->GetOriginTexture());
			m_resource_manager->GetParticleManager()->GetParticleSimulator()->SetLegendTextureId(m_legend_widget->GetOriginTexture());
		}
		if (RS_Input::mRSINPUT->IsTriggered(RSKey::N))
		{
			m_legend_widget->SetDividedValue(m_legend_widget->GetDividedValue() + 1);
			//m_resource_manager->GetParticleManager()->GetParticleSimulator()->SetDividedValue(m_legend_widget->GetDividedValue() + 1);
      m_resource_manager->GetParticleManager()->GetParticleSimulator()->SetLegendTextureId(m_legend_widget->GetDividedTexture());
		}
		if (RS_Input::mRSINPUT->IsTriggered(RSKey::M))
		{
			m_legend_widget->SetVertical(!m_legend_widget->GetVertical());
		}
		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::SOLVER_PAUSE)))
		{
			m_resource_manager->GetParticleManager()->GetParticleSimulator()->SwitchPlaying();

			if (m_resource_manager->GetParticleManager()->GetParticleSimulator()->IsPlaying())
			{
				// set invisible boundary box and setting box when playing
				m_resource_manager->GetParticleManager()->GetParticleSimulator()->SetHideObjectsStartToSolve();
			}
		}

		if (RS_Input::mRSINPUT->IsTriggered(RSKey::T))
		{
			auto PS = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
			PS->CallCalculateMinMaxValue();
		}

		if (RS_Input::mRSINPUT->IsTriggered(RSKey::U))
		{
			auto PS = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
			PS->SetNextRenderPropertyType();
		}

	}

	void ParticleSimulatorData::SetInitSettingParticleSystem(const RSInitSettingParticleSystem& init_setting)
	{
		m_particle_init_setting = init_setting;
	}

	void ParticleSimulatorData::SetParticleProperty(const RSParticleProperty& property)
	{
		m_particle_property = property;
	}


	void ParticleSimulatorData::Shutdown()
	{
#if USE_PLAYER

		auto player = m_resource_manager->GetObjectManager()->GetObject("Player");
		if (player != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(player);
#endif

		auto grid = m_resource_manager->GetObjectManager()->GetObject("Grid");
		if (grid != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(grid);


		// Remove objects from m_object_list
		while (!m_object_list.empty())
		{
			m_resource_manager->GetObjectManager()->RemoveObject(m_object_list.top());
			m_object_list.pop();
		}


		//auto cube = m_resource_manager->GetObjectManager()->GetObject("CollisionCube");
		//if (cube != nullptr)
		//	m_resource_manager->GetObjectManager()->RemoveObject(cube);

		/*auto CollisionObj1 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj1");
		if (CollisionObj1 != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(CollisionObj1);

		auto CollisionObj2 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj2");
		if (CollisionObj2 != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(CollisionObj2);

		auto CollisionObj3 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj3");
		if (CollisionObj3 != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(CollisionObj3);

		auto CollisionObj4 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj4");
		if (CollisionObj4 != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(CollisionObj4);

		auto CollisionObj5 = m_resource_manager->GetObjectManager()->GetObject("CollisionObj5");
		if (CollisionObj5 != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(CollisionObj5);*/

		if (m_legend_widget != nullptr)
			m_resource_manager->GetWidgetManager()->RemoveWidget(m_legend_widget->GetWidgetID());
		m_legend_widget = nullptr;

	}



}