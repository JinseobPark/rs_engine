/******************************************************************************/
/*!
\file   RSKeyBind.h
\author Jinseob Park
\date   2024/08/01

This file contains key binding class for key binding
This file includes key binding for key and mouse.
This file include Key list for key binding.

*/
/******************************************************************************/
#ifndef RS_KEY_BIND_H_
#define RS_KEY_BIND_H_

#include "pch.h"
#include <GLFW/glfw3.h>

/**
 * @brief Key List from GLFW
 */
enum class RSKey : std::int16_t
{
	SPACE = GLFW_KEY_SPACE,
	APOSTROPHE = GLFW_KEY_APOSTROPHE,
	COMMA = GLFW_KEY_COMMA,
	MINUS = GLFW_KEY_MINUS,
	PERIOD = GLFW_KEY_PERIOD,
	SLASH = GLFW_KEY_SLASH,
	NUM_0 = GLFW_KEY_0,
	NUM_1 = GLFW_KEY_1,
	NUM_2 = GLFW_KEY_2,
	NUM_3 = GLFW_KEY_3,
	NUM_4 = GLFW_KEY_4,
	NUM_5 = GLFW_KEY_5,
	NUM_6 = GLFW_KEY_6,
	NUM_7 = GLFW_KEY_7,
	NUM_8 = GLFW_KEY_8,
	NUM_9 = GLFW_KEY_9,
	SEMICOLON = GLFW_KEY_SEMICOLON,
	EQUAL = GLFW_KEY_EQUAL,
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,
	LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
	BACKSLASH = GLFW_KEY_BACKSLASH,
	RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
	GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
	WORLD_1 = GLFW_KEY_WORLD_1,
	WORLD_2 = GLFW_KEY_WORLD_2,
	ESCAPE = GLFW_KEY_ESCAPE,
	ENTER = GLFW_KEY_ENTER,
	TAB = GLFW_KEY_TAB,
	BACKSPACE = GLFW_KEY_BACKSPACE,
	INSERT = GLFW_KEY_INSERT,
	DEL = GLFW_KEY_DELETE,
	RIGHT = GLFW_KEY_RIGHT,
	LEFT = GLFW_KEY_LEFT,
	DOWN = GLFW_KEY_DOWN,
	UP = GLFW_KEY_UP,
	PAGE_UP = GLFW_KEY_PAGE_UP,
	PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
	HOME = GLFW_KEY_HOME,
	END = GLFW_KEY_END,
	CTRL_L = GLFW_KEY_LEFT_CONTROL,
	SHIFT_L = GLFW_KEY_LEFT_SHIFT,
	ALT_L = GLFW_KEY_LEFT_ALT,
	CTRL_R = GLFW_KEY_RIGHT_CONTROL,
	SHIFT_R = GLFW_KEY_RIGHT_SHIFT,
	ALT_R = GLFW_KEY_RIGHT_ALT,
	F1 = GLFW_KEY_F1,
	F2 = GLFW_KEY_F2,
	F3 = GLFW_KEY_F3,
	F4 = GLFW_KEY_F4,
	F5 = GLFW_KEY_F5,
	F6 = GLFW_KEY_F6,
	F7 = GLFW_KEY_F7,
	F8 = GLFW_KEY_F8,
	F9 = GLFW_KEY_F9,
	F10 = GLFW_KEY_F10,
	F11 = GLFW_KEY_F11,
	F12 = GLFW_KEY_F12,
	NONE = -1,
};

/**
 * @brief Mouse Button List from GLFW
 */
enum class RSMouse : std::int8_t
{
	LEFT = GLFW_MOUSE_BUTTON_LEFT,
	RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
	MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
	NONE = -1,
};

/**
 * @brief Key List for Key Binding
 */
enum class RSKeyList : std::int8_t
{
	QUIT,
	WIDGET_VIEW_TOGGLE,

	PLAYER_FORWARD,
	PLAYER_BACKWARD,
	PLAYER_LEFT,
	PLAYER_RIGHT,
	PLAYER_JUMP,
	PLAY_RESET,
	SWITCH_SKYBOX,

	PLAY_GAME_1,
	PLAY_GAME_2,
	PLAY_GAME_3,
	PLAY_GAME_4,
	PLAY_GAME_5,
	PLAY_GAME_6,
	PLAY_GAME_7,
	PLAY_GAME_8,
	PLAY_GAME_9,
	PLAY_GAME_10,
	PLAY_GAME_11,
	PLAY_GAME_12,

	PLAY_GAME_OUT,

	GIZMO_NONE,
	GIZMO_TRANSLATION,
	GIZMO_ROTATION,
	GIZMO_SCALE,

	CREATE_CUBE_OBJECT,
	CREATE_SPHERE_OBJECT,
	CREATE_PLANE_OBJECT,

	SELECT_ALL_OBJECT,
	COPY_OBJECT,
  PASTE_OBJECT,
  DUPLICATE_OBJECT,
	DELETE_OBJECT,

	SOLVER_PAUSE,
	FILE_DIALOG_OPEN,
	SAVE_FILE_DIALOG,
	TEST1,
	TEST2,
};

/**
 * @brief Input namespace
 */
namespace RS_Input
{
	/**
	 * @brief Key Binding class
	 */
	class RSKeyBind
	{
	public:
		/**
		 * @brief Constructor
		 */
		RSKeyBind();
		/**
		 * @brief Destructor
		 */
		~RSKeyBind();

		/**
		 * @brief Initialize Key Binding
		 */
		void Initialize();

		/**
		 * @brief Get Key from Key List
		 * @param name_ Key List name
		 * @return Key
		 */
		[[nodiscard]] RSKey GetKey(RSKeyList name_) const;

		/**
		 * @brief Set Key to Key List
		 * @param name_ Key List name
		 * @param key_ Key
		 */
		void SetKey(RSKeyList name_, RSKey key_);

		/**
		 * @brief Enroll Key to Key List
		 * @param name_ Key List name
		 * @param key_ Key
		 */
		void EnrollKey(RSKeyList name_, RSKey key_);

		/**
		 * @brief UnEnroll Key from Key List
		 * @param name_ Key List name
		 */
		void UnEnrollKey(RSKeyList name_);

	private:
    std::map<RSKeyList, RSKey> m_key_map; ///< Key map
	};
	extern RSKeyBind* mRSKEYBIND;
}

#endif // RS_KEY_BIND_H_