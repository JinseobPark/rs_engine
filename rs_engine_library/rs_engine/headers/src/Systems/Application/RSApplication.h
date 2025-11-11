/******************************************************************************/
/*!
\file   RSApplication.h
\author Jinseob Park
\date   2024/08/01

This file contains main application class for Game engine

*/
/******************************************************************************/
#ifndef RS_APPLICATION_H_
#define RS_APPLICATION_H_

#include "Systems/RSSystemComponent.h"

/**
 * @brief Application namespace
 */
namespace RS_Application
{
	/**
	 * @brief Application class
	 * This class derived from System Component
	 */
    class RSApplication final : public RSSystemComponent
    {
    public:
        /**
         * @brief Constructor.
         * Set Application static pointer to this class.
         */
        RSApplication();

		/**
		 * @brief Destructor.
		 */
        ~RSApplication() override;

        /**
         * @brief Initializer.
		 * Create GLFW window and set OpenGL version.
         */
        void Initialize() override;

        /**
         * @brief Update. Do nothing.
         * @param dt delta time.
         */
        void Update(float dt) override;

        /**
		 * @brief Shutdown. Destroy GLFW window.
         */
        void Shutdown() override;

		/**
		 * @brief Resize application window.
		 * @param width new width.
		 * @param height new height.
		 */
        void ResizeApp(int width, int height);

        /**
         * @brief Exit application status.
		 * @return true if exited.
         */
        bool IsExit() const { return glfwWindowShouldClose(p_window); }

        /**
         * @brief Get Window Width
		 * @return window width.
         */
        unsigned int GetWidth() const { return m_width; }

        /**
		 * @brief Get Window Height
		 * @return window height.
         */
        unsigned int GetHeight() const { return m_height; }

        /**
		 * @brief Set Window width and height.
         * @param width window width
         * @param height window height.
         */
        void SetWidthHeight(const unsigned int width, const unsigned int height) { m_width = width; m_height = height; }

        /**
         * @brief Get Window Pointer.
         * @return window pointer.
         */
        GLFWwindow* GetWindow() const { return p_window; } // need const or not


        /**
         * @brief Set window icon from png file path.
         * @param path png file path.
         */
        void SetWindowIcon(const char* path) const;
        
        /**
         * @brief Set window title.
         * @param title window title.
         */
        void SetWindowTitle(const char* title) const;

    private:
      unsigned int m_width = 1024; ///< Window width
      unsigned int m_height = 1024; ///< Window height

      const unsigned short OPENGL_MAJOR_VERSION = 4; ///< OpenGL major version
      const unsigned short OPENGL_MINOR_VERSION = 6; ///< OpenGL minor version

#ifdef _SPH_ENGINE
      bool v_sync = false; ///< V-sync
#else
        bool v_sync = true;
#endif
        GLFWwindow* p_window = nullptr; ///< GLFW window pointer
        bool is_fullscreen; ///< Fullscreen flag
    };
    extern RSApplication* mRSAPPLICATION; ///< Application pointer
}

#endif // !RS_APPLICATION_H_#endif // !RS_APPLICATION_H_