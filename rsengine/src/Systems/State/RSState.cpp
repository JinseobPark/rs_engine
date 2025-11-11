#include "pch.h"
#include "RSState.h"
#include "Resource/Manager/RSResourceManager.h"

namespace RS_State
{
	RSState* mRSSTATE = nullptr;

	RSState::RSState()
	{
		RS_ASSERT(mRSSTATE == nullptr, "Only One State Engine");
		mRSSTATE = this;

		m_current_state.Load = nullptr;
		m_current_state.Init = nullptr;
		m_current_state.Update = nullptr;
		m_current_state.Shutdown = nullptr;
		m_current_state.Unload = nullptr;
	}

	RSState::~RSState() = default;

	void RSState::Initialize()
	{
		// nothing
	}

	void RSState::PostInitialize()
	{
	}

	void RSState::Shutdown()
	{
		m_current_state.Shutdown();
		m_current_state.Unload();

		m_state_map.clear();
	}

	void RSState::Update(const float dt)
	{
		m_current_state.Update(dt);
	}

	void RSState::Restart() const
	{
		m_current_state.Shutdown();

		m_current_state.Init();
	}

	void RSState::AddState(const unsigned int state_id_, const RSStatement& state_)
	{
		// Check the state_ is valid
		if (state_.Init == nullptr || state_.Load == nullptr || state_.Shutdown == nullptr || state_.Unload == nullptr ||
        state_.Update == nullptr)
		{
			RS_FATAL("State struct is not valid. Please check the state struct.");
			return;
		}

		// Check the state_id_ is valid
		if (IsExistState(state_id_))
		{
			RS_FATAL("State ID is already exist. Please check the state ID.");
			return;
		}

		m_state_map[state_id_] = state_;
	}

	void RSState::SetNextState(const unsigned int state_id_)
	{
		if (IsExistState(state_id_))
		{
			// Shutdown and Unload the current state
			m_current_state.Shutdown();
			m_current_state.Unload();

			RSResourceManager::GetInstance()->GetObjectManager()->RemoveObjectsLevelChanged();

			// Change the state
			m_current_state_id = state_id_;
			m_current_state = m_state_map[state_id_];

			// Load and Init the new state
			m_current_state.Load();
			RSResourceManager::GetInstance()->ResetResourceLevel();

			m_current_state.Init();
		}

		_ASSERTE(IsExistState(state_id_) && "State ID is not exist. Please check the state ID.");
	}

	void RSState::SetStartState(const unsigned int state_id_)
	{
		if (IsExistState(state_id_))
		{
			m_current_state_id = state_id_;
			m_current_state = m_state_map[state_id_];

			m_current_state.Load();
			m_current_state.Init();
		}

		_ASSERTE(IsExistState(state_id_) && "State ID is not exist. Please check the state ID.");
	}

	void RSState::PrintStateList() const
	{
		for (auto& state : m_state_map)
			RS_DEBUG("State ID: %d", state.first);
	}

	bool RSState::IsExistState(const unsigned int state_id_) const
	{
		for (auto& state : m_state_map)
		{
			if(state.first == state_id_)
				return true;
		}
		return false;
	}

	
}
