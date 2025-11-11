#include "pch.h"
#include "RSEngine.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Timer/RSDebugTimer.h"
//#define GLEW_STATIC // This must be defined when statically linking to GLEW
#pragma comment (lib, "glew32s.lib") // Link to glew32s.lib

namespace RS_Engine
{
	RSEngine* mRSENGINE = nullptr;

	RSEngine::RSEngine()
	{
		m_logger = new RS_Logger::RSLogger();
		RS_ASSERT(mRSENGINE == nullptr, "Only One Render Engine");
		mRSENGINE = this;
		m_engine_last_time = static_cast<float>(glfwGetTime());
	}

	RSEngine::~RSEngine()
	{
#ifdef _SPH_DLL
		Shutdown();
#endif
		DestroyAllSystems();
		delete m_logger;

	}

	bool RSEngine::Initialize()
	{
		constexpr bool result = true;
		AddSystem(new RS_Application::RSApplication());
		AddSystem(new RS_Timer::RSTimer());
		AddSystem(new RS_Input::RSInputClass());
		AddSystem(new RS_Physics::RSPhysics());
		AddSystem(new RS_Event::RSEventHandler());
		//AddSystem(new LogicClass());
		AddSystem(new RS_Graphics::RSGraphics());
		AddSystem(new RS_State::RSState());



		for (const auto sys : m_systems)
		{
			sys->Initialize();
		}

		RSResourceManager::GetInstance()->Load();

		RSResourceManager::GetInstance()->Initialize();

		for (const auto sys : m_systems)
		{
			sys->PostInitialize();
		}
		RSResourceManager::GetInstance()->PostInitialize();

		is_running = result;
		return true;
	}

	void RSEngine::Run()
	{
		while (is_running)
		{
			is_running = Update();
		}
	}

	void RSEngine::DestroyAllSystems()
	{
		for (auto sys = m_systems.rbegin(); sys < m_systems.rend(); ++sys)
			delete* sys;
		m_systems.clear();
	}

	void RSEngine::Shutdown() const
	{
		for (const auto sys : m_systems)
		{
			sys->Shutdown();
		}
		RSResourceManager::GetInstance()->Shutdown();
		RSResourceManager::GetInstance()->Release();
    RS_Timer::RSDebugTimer::GetInstance()->Release();
	}

	bool RSEngine::Update()
	{
    const float dt = b_fixed_fps ? m_fixed_delta_time : RS_Timer::mRSTIMER->GetDeltaTime();
		if(!IsUpdateable())
      return is_running;
			
		b_update_able = false;

		//= RS_Timer::mRSTIMER->GetDeltaTime();
		//const float dt = 0.01f;
		RSResourceManager::GetInstance()->Update(dt);
		for (const auto sys : m_systems)
		{
			sys->Update(dt);
		}

		mRSEVENTHANDLER->UpdatePost(dt);
		RS_Graphics::mRSGRAPHICS->UpdatePost(dt);

		if (RS_Application::mRSAPPLICATION->IsExit())
			is_running = false;

		b_update_able = true;
		return is_running;
	}

	void RSEngine::SetFixedFps(const bool fixed_fps)
  {
    b_fixed_fps = fixed_fps;
    if (b_fixed_fps)
    {
      m_fixed_delta_time = 1.0f / m_fixed_fps;
    }
	}

	void RSEngine::SetFps(const float fps)
	{
		m_fixed_fps = fps;
		m_fixed_delta_time = 1.0f / m_fixed_fps;
	}

	void RSEngine::SetDeltaTime(const float dt)
	{
		m_fixed_delta_time = dt;
		m_fixed_fps = 1.0f / m_fixed_delta_time;
	}

	bool RSEngine::IsUpdateable()
	{
		if (!b_update_able)
			return false;
		if (!b_fixed_fps)
			return true;
		if (static_cast<float>(glfwGetTime()) - m_engine_last_time >= m_fixed_delta_time)
		{
			m_engine_last_time = static_cast<float>(glfwGetTime());
			return true;
		}
		return false;
	}

	//void RSEngine::ResizeApp(int width, int height)
	//{
	//	
	//	return SPH_VIEWER void();
	//}

	void RSEngine::AddSystem(RSSystemComponent* system)
	{
		m_systems.push_back(system);
	}
}
