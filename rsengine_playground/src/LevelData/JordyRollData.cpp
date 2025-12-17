#include "pch.h"
#include "JordyRollData.h"
#include "Util/RSUtilFunctions.h"
#include "GameData.h"

namespace SJG
{
	JordyRollData::JordyRollData()
	{
		m_player = nullptr;
		m_resource_manager = RSResourceManager::GetInstance();
	}

	JordyRollData::~JordyRollData()
	{
	}

	void JordyRollData::Initialize()
	{
		InitializePlayer();

		{ // macaron
			auto maca = m_resource_manager->GetObjectManager()->CreateObject("WeLuvMint");

			RS_Component::RSModel* maca_model = new RS_Component::RSModel();
			maca->AddComponent(maca_model);

			RS_Mesh::RSMesh* maca_mesh = new RS_Mesh::RSMesh();
			maca_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("macaron"));
			maca_mesh->SetTextureID(m_resource_manager->GetTextureManager()->GetTexture("mint_tex")->GetTexture());
			maca_mesh->SetMaterial(m_resource_manager->GetMaterialManager()->GetMaterial("mint_macaron"));
			maca_mesh->SetIsTexture(true);

			maca_model->SetMesh(maca_mesh);
			maca->GetTransform()->AddPosition(glm::vec3(0.0f, 1.0f, 0.0f));
		}

		{
			// Plane RSObject
			auto plane = m_resource_manager->GetObjectManager()->CreateObject("Grid");

			RS_Component::RSModel* plane_model = new RS_Component::RSModel();
			plane->AddComponent(plane_model);

			RS_Mesh::RSMesh* plane_mesh = new RS_Mesh::RSMesh();
			plane_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Grid"));
			plane_mesh->SetIsTexture(true);
			plane_mesh->SetTextureID(m_resource_manager->GetTextureManager()->GetTexture_UINT("jordy_alpha100"));

			plane_model->SetMesh(plane_mesh);
			plane_model->SetTilling(10.0f, 10.0f);
			plane->GetTransform()->SetScale(20.0f);
		}

	}

	void JordyRollData::InitializePlayer()
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
			player_mesh->SetTextureID(m_resource_manager->GetTextureManager()->GetTexture("jordy")->GetTexture());
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

		b_player_on_ground = true;
		m_player_speed = 5.0f;
	}


	void JordyRollData::Update(float dt)
	{
		// Lets Play the game
		m_player = m_resource_manager->GetObjectManager()->FindObject("Player");
		if (m_player != nullptr)
		{
		  
		_ASSERTE(m_player != nullptr && "Failed to find the player");

		RS_Component::RSRigidbody* player_rigidbody = static_cast<RS_Component::RSRigidbody*>(m_player->GetComponent(RSComponentType::CT_RIGIDBODY));
		_ASSERTE(player_rigidbody != nullptr && "Failed to find the rigidbody component from the player");
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

			if (m_player->GetTransform()->GetPosition().x < -9.0f)
			{
				player_position_temp.x = -9.0f;
				is_out_of_ground = true;
			}
			else if (m_player->GetTransform()->GetPosition().x > 9.0f)
			{
				player_position_temp.x = 9.0f;
				is_out_of_ground = true;
			}
			else if (m_player->GetTransform()->GetPosition().z < -9.0f)
			{
				player_position_temp.z = -9.0f;
				is_out_of_ground = true;
			}
			else if (m_player->GetTransform()->GetPosition().z > 9.0f)
			{
				player_position_temp.z = 9.0f;
				is_out_of_ground = true;
			}
			else
			{
				player_direction = auto_player_direction * distance;
				m_player->GetTransform()->AddPosition(player_direction);
				rotation_axis = glm::vec3(auto_player_direction.z, 0.0f, -auto_player_direction.x);
				const float rotation_angle = glm::length(player_direction);
				const glm::quat rotation = glm::angleAxis(rotation_angle, rotation_axis);
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

		}


		//auto plane = m_resource_manager->GetObjectManager()->CreateObject("Grid");
		// Get Grid object
    if (auto grid_obj = m_resource_manager->GetObjectManager()->FindObject("Grid"))
		{
		  // get model component of grid model
      auto grid_model = static_cast<RS_Component::RSModel*>(grid_obj->GetComponent(RSComponentType::CT_MODEL));
      glm::vec2 grid_offset_value = grid_model->GetOffset();
			grid_offset_value += glm::vec2(2.0f * dt, 0.0f);
      grid_offset_value = glm::mod(grid_offset_value, glm::vec2(1.0f));
      grid_model->SetOffset(grid_offset_value);
		}

		KeyUpdate(dt);

	}

	void JordyRollData::Shutdown()
	{
		auto player = m_resource_manager->GetObjectManager()->GetObject("Player");
		if (player != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(player);

		auto maca = m_resource_manager->GetObjectManager()->GetObject("WeLuvMint");
		if (maca != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(maca);

		auto grid = m_resource_manager->GetObjectManager()->GetObject("Grid");
		if (grid != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(grid);
	}

	void JordyRollData::KeyUpdate(float dt)
	{
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


		//if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_NONE)))
		//	RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::NONE);
		//if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_TRANSLATION)))
		//	RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::TRANSLATION);
		//if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_ROTATION)))
		//	RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::ROTATION);
		//if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_SCALE)))
		//	RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::SCALE);
	}
}