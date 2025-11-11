/*********************************************************************
 * \file   GameData.h
 * \brief  Game data for game logic
 * 
 * \author Jinseob Park
 * \date   2024/09/01
 *********************************************************************/
#pragma once

#include "GameStateManager.h"

namespace SJG
{
	static RSResourceManager* m_resource_manager = nullptr;
	class GameData
	{
	public:

		static GameData* GetInstance();
		static void Release();

		GameStateManager* GetGameStateManager() { return m_game_state_manager; }


		void Initialize();
		void Update(float dt);
		void Shutdown();

	private:

		GameData();
		~GameData();
		GameData(const GameData&);
		GameData& operator=(const GameData&) = delete;
		static GameData* mGameInstance;


		GameStateManager* m_game_state_manager = nullptr;
		//RS_Widget::WidgetLogic* m_widget_logic = nullptr;


	};

}