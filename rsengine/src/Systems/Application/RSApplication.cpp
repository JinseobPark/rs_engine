#include "pch.h"
#include "RSApplication.h"
#include "../Graphics/RSGraphics.h"
#include "Resource/Manager/RSResourceManager.h"


namespace RS_Application
{

	RSApplication* mRSAPPLICATION = nullptr;

	RSApplication::RSApplication()
	{
		assert(mRSAPPLICATION == nullptr && "No more than one instance of RSApplication system!");
		mRSAPPLICATION = this;
		is_fullscreen = false;
	}

	RSApplication::~RSApplication() = default;

	void RSApplication::Initialize()
	{
		if (!glfwInit()) {
			RS_FATAL("Failed to initialize GLFW");
			return;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);


		p_window = glfwCreateWindow(static_cast<int>(m_width), static_cast<int>(m_height), "Seob JJang", nullptr, nullptr);
		if (!p_window)
		{
			std::cout << "Failed to create the GLFW window\n";
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(p_window);

		auto framebuffer_size_callback = [](GLFWwindow* window, const int width, const int height)
	    {
	      mRSAPPLICATION->ResizeApp(width, height);
	      glViewport(0, 0, width, height);
	    };
		glfwSetFramebufferSizeCallback(p_window, framebuffer_size_callback);

		glfwSwapInterval(v_sync);
		glfwSetWindowUserPointer(p_window, this);

		GLenum err = glewInit();
		if (GLEW_OK != err) {
			RS_FATAL("Failed load GLEW");
			return;
		}
		glViewport(0, 0, static_cast<int>(m_width), static_cast<int>(m_height));
		ResizeApp(static_cast<int>(m_width), static_cast<int>(m_height));

	}

	void RSApplication::Update(float dt)
	{

	}

	void RSApplication::Shutdown()
	{
		glfwDestroyWindow(p_window);
		glfwTerminate();
	}

	void RSApplication::ResizeApp(const int width, const int height)
	{
		glfwSetWindowSize(p_window, width, height);
		m_width = width; m_height = height;
		//get resource manager, get camera
		RSResourceManager::GetInstance()->Resize(width, height);

		// Call SetRenderingFlagDirty() from graphics system to update
    RS_Graphics::mRSGRAPHICS->SetRenderingFlagDirty();

	}
	void RSApplication::SetWindowIcon(const char* path) const
  {
    const GLFWimage image = RSResourceManager::GetInstance()->GetTextureManager()->GetGlfwImageFromPath(path);
    if (image.pixels == nullptr)
    {
      RS_FATAL("Failed to load image for icon");
      return;
    }
    glfwSetWindowIcon(p_window, 1, &image);
    delete image.pixels;
	}
	void RSApplication::SetWindowTitle(const char* title) const
	{
    glfwSetWindowTitle(p_window, title);
	}
}
