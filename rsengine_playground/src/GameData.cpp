#include "pch.h"
#include "GameData.h"

namespace SJG
{
	GameData* GameData::mGameInstance = nullptr;

	GameData* GameData::GetInstance()
	{
		if (mGameInstance == nullptr)
		{
			mGameInstance = new GameData();
		}
		return mGameInstance;
	}

	void GameData::Release()
	{
		if (mGameInstance != nullptr)
		{
			delete mGameInstance;
			mGameInstance = nullptr;
		}
	}

	void GameData::Update(float dt)
	{
		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAY_GAME_1)))
		{
			RS_State::mRSSTATE->SetNextState(static_cast<unsigned int>(GameStateID::SRG_JORDY));
		}

		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAY_GAME_2)))
		{
			RS_State::mRSSTATE->SetNextState(static_cast<unsigned int>(GameStateID::SRG_PARTICLE_SIMULATOR));
		}

		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAY_GAME_3)))
		{
			RS_State::mRSSTATE->SetNextState(static_cast<unsigned int>(GameStateID::SRG_VTK_LOADER));
		}

		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAY_GAME_4)))
		{
			RS_State::mRSSTATE->SetNextState(static_cast<unsigned int>(GameStateID::SRG_POINT_CLIPPER));
		}

		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::PLAY_RESET)))
		{
			RS_State::mRSSTATE->Restart();
		}
	}


	void GameData::Initialize()
	{
	}

	void GameData::Shutdown()
	{
		delete m_game_state_manager;
	}

	GameData::GameData()
	{
		m_game_state_manager = new GameStateManager();
	}

	

	GameData::~GameData()
	{
	}

}