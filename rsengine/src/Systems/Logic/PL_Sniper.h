#pragma once

#include "Resource/RSImguiHandler.h"
#include "Resource/Manager/RSResourceManager.h"

namespace SPH_MINT
{
	class PL_Sniper
	{
	public:
		PL_Sniper();
		~PL_Sniper();

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

		bool is_play = false;
	};
}