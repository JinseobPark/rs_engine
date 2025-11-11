/******************************************************************************/
/*!
\file   RSInputClass.h
\author Jinseob Park
\date   2024/08/01

This file contains Input System for Game engine and logic part.

*/
/******************************************************************************/
#ifndef RS_INPUT_CLASS_H_
#define RS_INPUT_CLASS_H_

#include "Systems/RSSystemComponent.h"
#include "RSKeyBind.h"
#include "glm/glm.hpp"
#include <map>

/**
 * @brief GUI Handler namespace
 */
namespace RS_Handler
{
	class RSImguiHandler;
}

/**
 * @brief Key state enum class
 */
enum class RSKeyState : std::uint8_t
{
	NONE = 0, 
	RELEASED,
	PRESSED,
	HELD,
	BE_PRESSED,
	BE_RELEASED
};

/**
 * @brief Mouse scroll enum class
 */
enum class RSScroll : std::int8_t
{
	BE_UP = 2,
	UP = 1,
	NONE = 0,
	DOWN = -1,
	BE_DOWN = -2
};

/**
 * @brief Input namespace
 */
namespace RS_Input
{
	/**
	 * @brief Input Engine class
	 * This class derived from RSSystemComponent
	 */
	class RSInputClass : public RSSystemComponent
	{
	public:
		/**
		 * @brief Constructor.
		 * Allocate memory for key bind and initialize.
		 */
		RSInputClass();
		/**
		 * @brief Destructor.
		 * Delete memory for key bind.
		 */
		~RSInputClass() override;

		/**
		 * @brief Initialize glfw window and set callback function.
		 */
		void Initialize() override;

		/**
		 * @brief Update Key states and mouse states.
		 * Catch the key event and mouse event.
		 * @param dt Delta time.
		 */
		void Update(const float dt) override;

		/**
		 * @brief Shutdown glfw window.
		 */
		void Shutdown() override;

		/**
		 * @brief Check the key is triggered.
		 * @param key Check key.
		 * @return Return true if key is triggered.
		 */
		bool IsTriggered(const RSKey key);

		/**
		 * @brief Check the key is pressed.
		 * @param key Check key.
		 * @return Return true if key is pressed.
		 */
		bool IsPressed(const RSKey key);

		/**
		 * @brief Check the key is released.
		 * @param key Check key.
		 * @return Return true if key is released.
		 */
		bool IsReleased(const RSKey key);

		/**
		 * @brief Check the mouse button is triggered.
		 * @param button Check button.
		 * @return Return true if button is triggered.
		 */
		bool IsMouseButtonTriggered(const RSMouse button);

		/**
		 * @brief Check the mouse button is pressed.
		 * @param button Check button.
		 * @return Return true if button is pressed.
		 */
		bool IsMouseButtonPressed(const RSMouse button);

		/**
		 * @brief Check the mouse button is released.
		 * @param button Check button.
		 * @return Return true if button is released.
		 */
		bool IsMouseButtonReleased(const RSMouse button);

		/**
		 * @brief Check the mouse is scrolled.
		 * @param up_down Check scroll direction.
		 * @return Return true if mouse is scrolled.
		 */
		[[nodiscard]] bool IsMouseScrolled(const RSScroll up_down) const;

		/**
		 * @brief Get mouse scroll direction.
		 * @return Return mouse scroll direction.
		 */
		[[nodiscard]] RSScroll GetMouseScroll() const { return m_mouse_scrolled; }

		/**
		 * @brief Get mouse delta.
		 * @return Return mouse delta.
		 */
		[[nodiscard]] glm::ivec2 GetMouseDelta() const { return { m_mouse_dx, m_mouse_dy }; }

		/**
		 * @brief Get mouse position.
		 * @return Return mouse position.
		 */
		[[nodiscard]] glm::uvec2 GetMousePosition() const { return { m_mouse_x, m_mouse_y }; }

	protected:

		/**
		 * @brief Update key states.
		 */
		void UpdateKeyStates();

		/**
		 * @brief Update mouse states.
		 */
		void UpdateMouseStates();

		/**
		 * @brief Update mouse scroll.
		 */
		void UpdateMouseScroll();

		/**
		 * @brief Update mouse position.
		 */
		void UpdateMousePosition();

		/**
		 * @brief Update mouse delta.
		 */
		void UpdateCameraLogic(float dt);

		/**
		 * @brief Update picking frame buffer.
		 */
		void UpdatePickingFrameBuffer() const;

		/**
		 * @brief Update picking test.
		 */
		void UpdatePicking();

		/**
		 * @brief Update input logics.
		 * Shift & ESC : Close the window.
		 * Q : Switch the skybox. ( temp )
		 */
		void UpdateInputLogics();

	private:

    /**
     * @brief Mouse button callback function.
		 * @param window Glfw window pointer
		 * @param button mouse button code
     * @param action action code
     * @param mods mods code
		 */
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods); 
    /**
     * @brief Key callback function.
     * @param window Glfw window pointer
     * @param key key code
     * @param scancode scancode code
     * @param action action code
     * @param mods mods code
     */
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * @brief Cursor position callback function.
     * @param window Glfw window pointer
     * @param xpos cursor x position
     * @param ypos cursor y position
		 */
		static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

    /**
     * @brief Mouse scroll callback function.
     * @param window Glfw window pointer
     * @param xoffset mouse x offset
     * @param yoffset mouse y offset
		 */
		static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    /**
     * @brief Drop callback function.
     * @param window Glfw window pointer
     * @param count file count
     * @param paths file paths
     */
		static void DropCallback(GLFWwindow* window, int count, const char** paths);

    /**
     * @brief Handle key event.
		 * @param key key code
     * @param action action code
     * @param mods mods code
		 */
		void HandleKey(int key, int action, int mods);

    /**
     * @brief Handle mouse click event.
		 * @param button mouse button code
     * @param action action code
     * @param mods mods code
		 */
		void HandleMouseClick(int button, int action, int mods);

    /**
     * @brief Handle cursor position event.
     * @param xpos cursor x position
     * @param ypos cursor y position
     */
		void HandleCursorPosition(double xpos, double ypos);
    /**
     * @brief Handle mouse scroll event.
     * @param xoffset mouse x offset
     * @param yoffset mouse y offset
     */
		void HandleMouseScroll(double xoffset_, double yoffset_);


	private:
    GLFWwindow* p_window; ///< Glfw window pointer.
    unsigned int m_mouse_x = 0; ///< Mouse x position.
    unsigned int m_mouse_y = 0; ///< Mouse y position.
    int m_mouse_dx = 0, m_mouse_dy = 0; ///< Mouse delta.
    bool is_mouse_moved = false; ///< Check mouse moved or not.
    std::unordered_map<RSKey, RSKeyState> m_key_states; ///< Key states.
    std::unordered_map<RSMouse, RSKeyState> m_mouse_button_states; ///< Mouse button states.
	
    RSScroll m_mouse_scrolled = RSScroll::NONE; ///< Mouse scroll state.

    RSKeyBind* m_key_bind; ///< Key bind pointer.

#ifndef IS_CONTROL_IMGUI_VIEWPORT
    bool in_final_viewport = false; ///< Check final viewport.
#endif
		friend class RS_Handler::RSImguiHandler;
	};
	extern RSInputClass* mRSINPUT;
}

#endif // RS_INPUT_CLASS_H_