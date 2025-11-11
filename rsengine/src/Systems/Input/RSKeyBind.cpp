#include "pch.h"
#include "RSKeyBind.h"
#include <cassert>

namespace RS_Input
{
	RSKeyBind* mRSKEYBIND = nullptr;
	RSKeyBind::RSKeyBind()
	{
		RS_ASSERT(mRSKEYBIND == nullptr, "No more than one instance of RSKeyBind system!");
		mRSKEYBIND = this;
	}

	RSKeyBind::~RSKeyBind() = default;

	void RSKeyBind::Initialize()
	{
		EnrollKey(RSKeyList::QUIT, RSKey::ESCAPE);
		EnrollKey(RSKeyList::PLAYER_FORWARD, RSKey::W);
		EnrollKey(RSKeyList::PLAYER_BACKWARD, RSKey::S);
		EnrollKey(RSKeyList::PLAYER_LEFT, RSKey::A);
		EnrollKey(RSKeyList::PLAYER_RIGHT, RSKey::D);
		EnrollKey(RSKeyList::PLAYER_JUMP, RSKey::SPACE);
		EnrollKey(RSKeyList::PLAY_RESET, RSKey::R);
		EnrollKey(RSKeyList::SWITCH_SKYBOX, RSKey::Q);
		EnrollKey(RSKeyList::WIDGET_VIEW_TOGGLE, RSKey::I);

    EnrollKey(RSKeyList::COPY_OBJECT, RSKey::C);
    EnrollKey(RSKeyList::PASTE_OBJECT, RSKey::V);
    EnrollKey(RSKeyList::DUPLICATE_OBJECT, RSKey::D);
    EnrollKey(RSKeyList::SELECT_ALL_OBJECT, RSKey::A);

		EnrollKey(RSKeyList::DELETE_OBJECT, RSKey::DEL);
		EnrollKey(RSKeyList::CREATE_CUBE_OBJECT, RSKey::J);
		EnrollKey(RSKeyList::CREATE_SPHERE_OBJECT, RSKey::K);
		EnrollKey(RSKeyList::CREATE_PLANE_OBJECT, RSKey::L);

		EnrollKey(RSKeyList::PLAY_GAME_1, RSKey::F1);
		EnrollKey(RSKeyList::PLAY_GAME_2, RSKey::F2);
		EnrollKey(RSKeyList::PLAY_GAME_3, RSKey::F3);
		EnrollKey(RSKeyList::PLAY_GAME_4, RSKey::F4);
		EnrollKey(RSKeyList::PLAY_GAME_5, RSKey::F5);
		EnrollKey(RSKeyList::PLAY_GAME_6, RSKey::F6);
		EnrollKey(RSKeyList::PLAY_GAME_7, RSKey::F7);
		EnrollKey(RSKeyList::PLAY_GAME_8, RSKey::F8);
		EnrollKey(RSKeyList::PLAY_GAME_9, RSKey::F9);
		EnrollKey(RSKeyList::PLAY_GAME_10, RSKey::F10);
		EnrollKey(RSKeyList::PLAY_GAME_11, RSKey::F11);
		EnrollKey(RSKeyList::PLAY_GAME_OUT, RSKey::END);
		

		EnrollKey(RSKeyList::GIZMO_NONE, RSKey::NUM_1);
		EnrollKey(RSKeyList::GIZMO_TRANSLATION, RSKey::NUM_2);
		EnrollKey(RSKeyList::GIZMO_ROTATION, RSKey::NUM_3);
		EnrollKey(RSKeyList::GIZMO_SCALE, RSKey::NUM_4);

		EnrollKey(RSKeyList::SOLVER_PAUSE, RSKey::P);
		EnrollKey(RSKeyList::FILE_DIALOG_OPEN, RSKey::O);
    EnrollKey(RSKeyList::SAVE_FILE_DIALOG, RSKey::S);



	}

	RSKey RSKeyBind::GetKey(const RSKeyList name_) const
	{
		const auto it = m_key_map.find(name_);
		if (it != m_key_map.end()) {
			return it->second;
		}
		return RSKey::NONE;
	}
	void RSKeyBind::SetKey(const RSKeyList name_, const RSKey key_)
	{
		const auto it = m_key_map.find(name_);
		if (it != m_key_map.end()) {
			it->second = key_;
		}
	}
	void RSKeyBind::EnrollKey(RSKeyList name_, RSKey key_)
	{
		const auto it = m_key_map.find(name_);
		if (it != m_key_map.end()) {
			it->second = key_;
		}
		else {
			m_key_map.insert(std::make_pair(name_, key_));
		}
	}
	void RSKeyBind::UnEnrollKey(const RSKeyList name_)
	{
		const auto it = m_key_map.find(name_);
		if (it != m_key_map.end()) {
			m_key_map.erase(it);
		}
	}
}
