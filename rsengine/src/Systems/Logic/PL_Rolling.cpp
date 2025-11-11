#include "pch.h"
#include "PL_Rolling.h"
#include "systems/Input/RSInputClass.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/constants.hpp"
#include "Resource/Object/Component/RSComponentHeader.h"
#include "Util/RSUtilFunctions.h"
#include <iostream>

namespace SPH_MINT
{
	using RS_Input::mRSINPUT;
	using RS_Input::mRSKEYBIND;

	using namespace RS_Object;
	PL_Rolling::PL_Rolling()
	{
	}

	PL_Rolling::~PL_Rolling()
	{
	}

	void PL_Rolling::Initialize()
	{
	}

	void PL_Rolling::PostInitialize()
	{
		m_resource_manager = RSResourceManager::GetInstance();
	}

	void PL_Rolling::PlayInitializer()
	{
		if(is_play) return;
		{ // Player
			m_resource_manager->GetObjectManager()->CreateObject("Player");
			auto player = m_resource_manager->GetObjectManager()->GetObject("Player");

			// if player does not have model component, create one and add this
			if (!player->GetComponent(RSComponentType::CT_MODEL))
			{
				RSModel* player_model = new RSModel();
				player->AddComponent(player_model);

				RSMesh* player_mesh = new RSMesh();
				player_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Sphere"));
				player_mesh->SetTextureID(m_resource_manager->GetTextureManager()->GetTexture("jordy")->GetTexture());
				player_mesh->SetIsTexture(true);

				player_model->SetMesh(player_mesh);
			}

			if (!player->GetComponent(RSComponentType::CT_RIGIDBODY))
			{
				RSRigidbody* player_rigidbody = new RSRigidbody();
				player->AddComponent(player_rigidbody);
			}

			player->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));


		}
		is_play = true;
	}

	void PL_Rolling::PlayShutdown()
	{
		if(!is_play) return;

		auto player = m_resource_manager->GetObjectManager()->GetObject("Player");
		if(player != nullptr)
			m_resource_manager->GetObjectManager()->RemoveObject(player);
		is_play = false;
	}

	void PL_Rolling::Reset()
	{
		RSObject* Player = RSResourceManager::GetInstance()->GetObjectManager()->GetObject("Player");
		if (Player == nullptr) return;
		RSRigidbody* player_rigidbody = static_cast<RSRigidbody*>(Player->GetComponent(RSComponentType::CT_RIGIDBODY));
		if (player_rigidbody == nullptr)  return;

		Player->GetTransform()->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
		Player->GetTransform()->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));

		b_player_on_ground = true;
		m_player_speed = 5.0f;
	}

	void PL_Rolling::Update(float dt)
	{
		// Lets Play the game
		RSObject* Player = RSResourceManager::GetInstance()->GetObjectManager()->GetObject("Player");

		if (Player == nullptr) return;
		RSRigidbody* player_rigidbody = static_cast<RSRigidbody*>(Player->GetComponent(RSComponentType::CT_RIGIDBODY));
		if (player_rigidbody == nullptr)  return;
		// when player on the ground, player's rigidbody should be zero
		if (Player->GetTransform()->GetPosition().y <= 1.0f)
		{
			Player->GetTransform()->SetPosition(glm::vec3(Player->GetTransform()->GetPosition().x, 1.0f, Player->GetTransform()->GetPosition().z));
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
			glm::vec3 player_position_temp = Player->GetTransform()->GetPosition();
			bool is_out_of_ground = false;

			if (Player->GetTransform()->GetPosition().x < -9.0f)
			{
				player_position_temp.x = -9.0f;
				is_out_of_ground = true;
			}
			else if (Player->GetTransform()->GetPosition().x > 9.0f)
			{
				player_position_temp.x = 9.0f;
				is_out_of_ground = true;
			}
			else if (Player->GetTransform()->GetPosition().z < -9.0f)
			{
				player_position_temp.z = -9.0f;
				is_out_of_ground = true;
			}
			else if (Player->GetTransform()->GetPosition().z > 9.0f)
			{
				player_position_temp.z = 9.0f;
				is_out_of_ground = true;
			}
			else
			{
				/*
				player_direction = glm::normalize(player_direction);
				player_direction *= distance;
				Player->GetTransform()->AddPosition(player_direction);
				// Calculate the rotation angle
				// Formula : l = r * theta ,r = 1, l = player_direction
				float rotation_angle = glm::length(player_direction);//*2.0f / glm::pi<float>();
				glm::vec3 normalized_axis = glm::normalize(rotation_axis);
				if (glm::length(normalized_axis) != 0.0f)
				{
					glm::quat rotation = glm::angleAxis(rotation_angle, normalized_axis);
					Player->GetTransform()->AddRotation(rotation);
				}
				*/
				player_direction = auto_player_direction * distance;
				Player->GetTransform()->AddPosition(player_direction);
				rotation_axis = glm::vec3(auto_player_direction.z, 0.0f, -auto_player_direction.x);
				float rotation_angle = glm::length(player_direction);
				glm::quat rotation = glm::angleAxis(rotation_angle, rotation_axis);
				Player->GetTransform()->AddRotation(rotation);
			}

			if (is_out_of_ground)
			{
				Player->GetTransform()->SetPosition(player_position_temp);
				// New Random direction
				float random_x = RS_Util::GetRandomNumberExcludeZero();
				float random_z = RS_Util::GetRandomNumberExcludeZero();
				auto_player_direction = normalize(glm::vec3(random_x, 0.0f, random_z));
			}
		}
		else
		{
			// Player control with "WASD" key to move X,Z axis
			if (mRSINPUT->IsPressed(mRSKEYBIND->GetKey(RSKeyList::PLAYER_FORWARD)))
			{
				player_direction.z = -1.0f;
				rotation_axis.x = -1.0f;
			}
			if (mRSINPUT->IsPressed(mRSKEYBIND->GetKey(RSKeyList::PLAYER_BACKWARD)))
			{
				player_direction.z = 1.0f;
				rotation_axis.x = 1.0f;
			}
			if (mRSINPUT->IsPressed(mRSKEYBIND->GetKey(RSKeyList::PLAYER_LEFT)))
			{
				player_direction.x = -1.0f;
				rotation_axis.z = 1.0f;
			}
			if (mRSINPUT->IsPressed(mRSKEYBIND->GetKey(RSKeyList::PLAYER_RIGHT)))
			{
				player_direction.x = 1.0f;
				rotation_axis.z = -1.0f;
			}
			if (mRSINPUT->IsPressed(mRSKEYBIND->GetKey(RSKeyList::PLAYER_JUMP)))
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
				Player->GetTransform()->AddPosition(player_direction);
				// Calculate the rotation angle
				// Formula : l = r * theta ,r = 1, l = player_direction
				float rotation_angle = glm::length(player_direction);//*2.0f / glm::pi<float>();
				glm::vec3 normalized_axis = glm::normalize(rotation_axis);
				if (glm::length(normalized_axis) != 0.0f)
				{
					glm::quat rotation = glm::angleAxis(rotation_angle, normalized_axis);
					Player->GetTransform()->AddRotation(rotation);
				}
			}
		}
		


		if (mRSINPUT->IsTriggered(mRSKEYBIND->GetKey(RSKeyList::SOLVER_PAUSE)))
		{
			RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->SwitchPlaying();
		}


		if (mRSINPUT->IsTriggered(mRSKEYBIND->GetKey(RSKeyList::PLAY_RESET)))
		{
			Reset();
			if (RSResourceManager::GetInstance()->GetParticleManager()->IsUseSPH())
				RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->ResetParticle();

		}
		if (mRSINPUT->IsTriggered(RSKey::INSERT))
		{
			is_auto_play = !is_auto_play;
		}

	}

	void PL_Rolling::Shutdown()
	{
	}
}

