#include "pch.h"
#include "RSInputClass.h"
#include "../Application/RSApplication.h"
#include "Resource/Manager/RSResourceManager.h"
#include <locale>
#include <codecvt>
#include <filesystem>
//#pragma execution_character_set("utf-8")

namespace RS_Input
{
	using RS_Application::mRSAPPLICATION;

	RSInputClass* mRSINPUT = nullptr;

	RSInputClass::RSInputClass()
	{
		RS_ASSERT(mRSINPUT == nullptr, "No more than one instance of Input system!");
		mRSINPUT = this;
		p_window = nullptr;

		m_key_bind = new RSKeyBind();
	}

	RSInputClass::~RSInputClass()
	{
		delete m_key_bind;
	}


	void RSInputClass::Initialize()
	{
		p_window = mRSAPPLICATION->GetWindow();

		glfwSetDropCallback(p_window, DropCallback);
		glfwSetMouseButtonCallback(p_window, MouseButtonCallback);
		glfwSetKeyCallback(p_window, KeyCallback);
		glfwSetCursorPosCallback(p_window, CursorPositionCallback);
		glfwSetScrollCallback(p_window, MouseScrollCallback);
		glfwSetWindowUserPointer(p_window, this);

		m_key_bind->Initialize();
	}

	void RSInputClass::Shutdown()
	{
	}

	void RSInputClass::UpdateKeyStates()
	{
		for (auto& [key_id, key_state] : m_key_states) {
			if (key_state == RSKeyState::BE_PRESSED) {
				key_state = RSKeyState::PRESSED;
			}
			else if (key_state == RSKeyState::BE_RELEASED) {
				key_state = RSKeyState::RELEASED;
			}
			else if (key_state == RSKeyState::PRESSED) {
				key_state = RSKeyState::HELD;
			}
		}
	}

	void RSInputClass::UpdateMouseStates()
	{
		for (auto& [mouse_id, mouse_state] : m_mouse_button_states) {
			if (mouse_state == RSKeyState::BE_PRESSED) {
				mouse_state = RSKeyState::PRESSED;
			}
			else if (mouse_state == RSKeyState::BE_RELEASED) {
				mouse_state = RSKeyState::RELEASED;
			}
			else if (mouse_state == RSKeyState::PRESSED) {
				mouse_state = RSKeyState::HELD;
			}
		}
	}

	void RSInputClass::UpdateMouseScroll()
	{
		if (m_mouse_scrolled == RSScroll::BE_UP)
			m_mouse_scrolled = RSScroll::UP;
		else if (m_mouse_scrolled == RSScroll::BE_DOWN)
			m_mouse_scrolled = RSScroll::DOWN;
		else if (m_mouse_scrolled == RSScroll::UP || m_mouse_scrolled == RSScroll::DOWN)
			m_mouse_scrolled = RSScroll::NONE;
	}

	void RSInputClass::UpdateMousePosition()
	{
		if (is_mouse_moved)
		{
			is_mouse_moved = false;
			RSResourceManager::GetInstance()->GetGraphicsData()->skybox_timer_value = SKYBOX_TIMER_VALUE; // IDEA : JupiterJY
		}
		else
		{
			m_mouse_dx = 0;
			m_mouse_dy = 0;
		}
	}

	void RSInputClass::UpdateCameraLogic(const float dt)
	{
    constexpr float rot_speed = 0.01f;
    constexpr float move_speed = 0.0005f;

		// Camera Logics
		if (IsMouseButtonPressed(RSMouse::RIGHT)) //HOLD
			RSResourceManager::GetInstance()->GetCamera()->Rotate(
				rot_speed * static_cast<float>(mRSINPUT->GetMouseDelta().x), 
				rot_speed * static_cast<float>(mRSINPUT->GetMouseDelta().y));

		if (IsMouseButtonPressed(RSMouse::MIDDLE)) //HOLD
			RSResourceManager::GetInstance()->GetCamera()->Move(
				move_speed * static_cast<float>(mRSINPUT->GetMouseDelta().x), 
				move_speed * static_cast<float>(mRSINPUT->GetMouseDelta().y));

		if (GetMouseScroll() != RSScroll::NONE)
			RSResourceManager::GetInstance()->GetCamera()->Zoom(static_cast<int>(mRSINPUT->GetMouseScroll()));
	}

	void RSInputClass::UpdatePickingFrameBuffer() const
	{
		// Update picking pixel id
    const auto picking_fbo = dynamic_cast<_RS_Internal::RSPickingFbo*>(RSResourceManager::GetInstance()->GetBufferManager()->GetFboItem(FboType::PICKING));
		glBindFramebuffer(GL_FRAMEBUFFER, picking_fbo->GetPickingFrameBuffer());
		glReadPixels(static_cast<int>(m_mouse_x), static_cast<int>(mRSAPPLICATION->GetHeight() - m_mouse_y), 1, 1,
                 GL_RED_INTEGER, GL_UNSIGNED_INT, &RSResourceManager::GetInstance()->GetGraphicsData()->m_pixel_id);
		RSResourceManager::GetInstance()->GetBufferManager()->UnbindFbo(FboType::PICKING);
		// m_pixel_id means that it is on widget.
	}

	void RSInputClass::UpdatePicking()
	{
		if (IsMouseButtonTriggered(RSMouse::LEFT))
		{
#ifdef _SPH_DEBUG
			if (RSResourceManager::GetInstance()->GetGraphicsData()->m_pixel_id != 0)
				RS_INFO("Picked RSObject ID : %d", RSResourceManager::GetInstance()->GetGraphicsData()->m_pixel_id);
			else
				RS_INFO("No RSObject Picked");
#endif
			//"if m_pixel_id is 0, then work."
			if (RSResourceManager::GetInstance()->GetGraphicsData()->m_pixel_id != 0)
			{
				// Widget Clicked
				RSResourceManager::GetInstance()->GetWidgetManager()->
                                          Clicked(RSResourceManager::GetInstance()->GetGraphicsData()->m_pixel_id);
			}
			else
			{
#ifdef _SPH_DEBUG
				RS_INFO("Mouse Clicked : X = %d, Y = %d", m_mouse_x, m_mouse_y);
#endif
				RSResourceManager::GetInstance()->GetObjectManager()->SelectObjectByMousePicking(static_cast<int>(m_mouse_x),
          static_cast<int>(m_mouse_y), IsPressed(RSKey::CTRL_L));
			}

		}

		if (IsMouseButtonReleased(RSMouse::LEFT))
		{
			RSResourceManager::GetInstance()->GetWidgetManager()->Released();
			// TODO : send message to obj manager
			RSResourceManager::GetInstance()->GetObjectManager()->ReleasePicking();
#ifdef _SPH_DEBUG
			RS_INFO("Mouse Released : X = %d, Y = %d", m_mouse_x, m_mouse_y);
#endif
		}
	}

	void RSInputClass::UpdateInputLogics()
	{
		if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::DELETE_OBJECT)))
			RSResourceManager::GetInstance()->GetObjectManager()->CalledRemoveObject();

		if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::CREATE_CUBE_OBJECT)) && IsPressed(RSKey::CTRL_L))
			RSResourceManager::GetInstance()->GetObjectManager()->CreateCubeObject();

		if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::CREATE_SPHERE_OBJECT)) && IsPressed(RSKey::CTRL_L))
			RSResourceManager::GetInstance()->GetObjectManager()->CreateSphereObject();

		if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::CREATE_PLANE_OBJECT)) && IsPressed(RSKey::CTRL_L))
			RSResourceManager::GetInstance()->GetObjectManager()->CreatePlaneObject();

    // Copy & Paste with Ctrl + C, Ctrl + V
    if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::COPY_OBJECT)) && IsPressed(RSKey::CTRL_L))
      RSResourceManager::GetInstance()->GetObjectManager()->SetCopyObjectsToName(RSResourceManager::GetInstance()->GetObjectManager()->GetPickedObjectList());

    if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::PASTE_OBJECT)) && IsPressed(RSKey::CTRL_L))
      RSResourceManager::GetInstance()->GetObjectManager()->PasteTheCopiedObjects();

    // Duplicate object with Ctrl + D
		if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::DUPLICATE_OBJECT)) && IsPressed(RSKey::CTRL_L))
			RSResourceManager::GetInstance()->GetObjectManager()->DuplicateObjects();

    // Select all objects with Ctrl + A`
		if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::SELECT_ALL_OBJECT)) && IsPressed(RSKey::CTRL_L))
			RSResourceManager::GetInstance()->GetObjectManager()->SelectAllObjects();

		// Switching Object visibility with 'H'
		if (IsTriggered(RSKey::H))
		{
		  // Get picked objects from object manager

      // Switch visibility to model component at picked objects
			for (const auto picked_objects = RSResourceManager::GetInstance()->GetObjectManager()->GetPickedObjectList(); const auto obj : picked_objects)
      {
        if (const auto model = dynamic_cast<RS_Component::RSModel*>(obj->GetComponent(RSComponentType::CT_MODEL)); model != nullptr)
					model->SetVisible(!model->GetIsVisible());
      }
		}



		// GIZMO update.
		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_NONE)))
			RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::NONE);
		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_TRANSLATION)))
			RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::TRANSLATION);
		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_ROTATION)))
			RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::ROTATION);
		if (RS_Input::mRSINPUT->IsTriggered(RS_Input::mRSKEYBIND->GetKey(RSKeyList::GIZMO_SCALE)))
			RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::SCALE);

#ifdef _SPH_ENGINE
		if (IsTriggered(mRSKEYBIND->GetKey(RSKeyList::QUIT)) && IsPressed(RSKey::SHIFT_L))
			glfwSetWindowShouldClose(mRSAPPLICATION->GetWindow(), true);
#endif // _SPH_ENGINE
	}

	void RSInputClass::Update(const float dt)
	{
		UpdateKeyStates();
		UpdateMouseStates();
		UpdateMouseScroll();
		UpdateMousePosition();
		UpdateCameraLogic(dt);
		UpdatePickingFrameBuffer();
		UpdatePicking();
		UpdateInputLogics();

	}

	void RSInputClass::MouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
	{
		if (mRSINPUT != nullptr) {
			mRSINPUT->HandleMouseClick(button, action, mods);
		}
	}

	void RSInputClass::KeyCallback(GLFWwindow* window, const int key, int scancode, const int action, const int mods)
	{
		if (mRSINPUT != nullptr) {
			mRSINPUT->HandleKey(key, action, mods);
		}
	}

	void RSInputClass::CursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
	{
		if (mRSINPUT != nullptr) {
			// Check xpos, ypos are in the window
			if (xpos >= 0 && xpos <= mRSAPPLICATION->GetWidth() && ypos >= 0 && ypos <= mRSAPPLICATION->GetHeight())
				mRSINPUT->HandleCursorPosition(xpos, ypos);
		}
	}

	void RSInputClass::MouseScrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
	{
		if (mRSINPUT != nullptr) {
			mRSINPUT->HandleMouseScroll(xoffset, yoffset);
		}
	}

	void RSInputClass::DropCallback(GLFWwindow* window, const int count, const char** paths)
	{
		for (int i = 0; i < count; i++)
		{
			// utf8 ? Hangul 3byte
			// wchar_t 2byte
			// So, convert to wstring from utf-8 at c++20
			// std::filesystem::path::u8string()
			// std::filesystem::path::wstring()
			// 
			// TODO : Import any fucking Hangul file
			// convert to wstring from utf-8

			std::string get_path_str = paths[i];
			if(get_path_str.empty())
				continue;

			// convert paths to wstring
      std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
      std::wstring get_path_wstr = converter.from_bytes(get_path_str);


			RSResourceManager::GetInstance()->GetFileIOManager()->OpenFile(get_path_wstr);
			//RSResourceManager::GetInstance()->GetFileIOManager()->OpenFile(get_path_str);
			RS_INFO("Dropped File : %s", paths[i]);
			//std::cout << "Dropped File : " << paths[i] << std::endl;
		}
	}

	void RSInputClass::HandleKey(int key, const int action, int mods)
	{
		if (action == GLFW_PRESS) {
			m_key_states[static_cast<RSKey>(key)] = RSKeyState::BE_PRESSED;
			//std::cout << "Pressed Input : " << key << std::endl;
		}
		else if (action == GLFW_RELEASE) {
			m_key_states[static_cast<RSKey>(key)] = RSKeyState::BE_RELEASED;
			//std::cout << "Released Input : " << key << std::endl;
		}
	}

	void RSInputClass::HandleMouseClick(int button, const int action, int mods)
	{
		if (action == GLFW_PRESS) {
			m_mouse_button_states[static_cast<RSMouse>(button)] = RSKeyState::BE_PRESSED;
			//std::cout << button << " Pressed Input : X = " << m_mouseX << ", Y = " << m_mouseY << std::endl;
		}
		else if (action == GLFW_RELEASE) {
			m_mouse_button_states[static_cast<RSMouse>(button)] = RSKeyState::BE_RELEASED;
			//std::cout << button << " Released Input : X = " << m_mouseX << ", Y = " << m_mouseY << std::endl;
		}
	}

	bool RSInputClass::IsTriggered(const RSKey key)
	{
		return m_key_states[key] == RSKeyState::PRESSED;
	}

	bool RSInputClass::IsPressed(const RSKey key)
	{
		return m_key_states[key] == RSKeyState::PRESSED || m_key_states[key] == RSKeyState::HELD;
	}

	bool RSInputClass::IsReleased(const RSKey key)
	{
		if (m_key_states[key] == RSKeyState::RELEASED) {
			m_key_states[key] = RSKeyState::NONE;
			return true;
		}
		return false;
	}

	void RSInputClass::HandleCursorPosition(const double xpos, const double ypos)
	{
		is_mouse_moved = true;
		m_mouse_dx = static_cast<int>(m_mouse_x) - static_cast<int>(xpos);
		m_mouse_dy = static_cast<int>(m_mouse_y) - static_cast<int>(ypos);
		m_mouse_x = std::clamp(static_cast<int>(xpos), 0, static_cast<int>(RS_Application::mRSAPPLICATION->GetWidth()));
		m_mouse_y = std::clamp(static_cast<int>(ypos), 0, static_cast<int>(RS_Application::mRSAPPLICATION->GetHeight()));
	}

	void RSInputClass::HandleMouseScroll(double xoffset_, const double yoffset_)
	{
		// only yoffset use.
		if (yoffset_ > 0)
			m_mouse_scrolled = RSScroll::BE_UP;
		else
			m_mouse_scrolled = RSScroll::BE_DOWN;
	}

	bool RSInputClass::IsMouseButtonTriggered(const RSMouse button)
	{
		return m_mouse_button_states[button] == RSKeyState::PRESSED;
	}

	bool RSInputClass::IsMouseButtonPressed(const RSMouse button)
	{
		return m_mouse_button_states[button] == RSKeyState::PRESSED || m_mouse_button_states[button] == RSKeyState::HELD;
	}

	bool RSInputClass::IsMouseButtonReleased(const RSMouse button)
	{
		if (m_mouse_button_states[button] == RSKeyState::RELEASED) {
			m_mouse_button_states[button] = RSKeyState::NONE;
			return true;
		}
		return false;
	}

	bool RSInputClass::IsMouseScrolled(const RSScroll up_down) const
	{
		// m_mouse_scrolled : 1, -1
		if (up_down == m_mouse_scrolled) return true;

		return false;
	}
}


