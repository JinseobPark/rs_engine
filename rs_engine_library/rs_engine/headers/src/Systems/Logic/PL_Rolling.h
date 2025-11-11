#pragma once
#include "Resource/RSImguiHandler.h"
#include "Resource/Manager/RSResourceManager.h"

namespace SPH_MINT
{
	class PL_Rolling
	{
		public:
		PL_Rolling();
		~PL_Rolling();

		void Initialize();

		void PostInitialize();

		void PlayInitializer();
		void PlayShutdown();

		void Reset();

		void Update(float dt);

		void Shutdown();

		bool IsPlay() { return is_play; }

	private:
		RSResourceManager* m_resource_manager = nullptr;
		float m_player_speed = 5.0f;
		bool b_player_on_ground = true;
		friend class RS_Handler::RSImguiHandler;

		bool is_auto_play = true;
		float auto_jump_time = 2.0f;
		glm::vec3 auto_player_direction = glm::vec3(1.0f, 0.0f, 0.0f);

		bool is_play = false;
	};
}
