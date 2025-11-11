#include "pch.h"
#include "RSDebugTimer.h"

namespace RS_Timer
{
	RSDebugTimer* RSDebugTimer::m_instance = nullptr;

	RSDebugTimer* RSDebugTimer::GetInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = new RSDebugTimer();
		}
		return m_instance;
	}

	void RSDebugTimer::Release()
	{
		if (m_instance != nullptr)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	}

	void RSDebugTimer::Start()
	{
		start_time = std::chrono::system_clock::now();
	}

	void RSDebugTimer::EndNPrint()
	{
		end_time = std::chrono::system_clock::now();

		elapsed_time = end_time - start_time;
		RS_DEBUG("Elapsed Time : %lld ms", std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count());
		//std::cout << "Elapsed Time : " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() << " ms\n";
	}

	void RSDebugTimer::EndNPrint(const std::string& message_)
	{
		end_time = std::chrono::system_clock::now();

		elapsed_time = end_time - start_time;
		RS_DEBUG("%s : %lld ms", message_.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count());
		//std::cout << message_ << " : " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count() << " ms\n";
	}

	void RSDebugTimer::EndNLog(const std::string& message_)
	{
		end_time = std::chrono::system_clock::now();

		elapsed_time = end_time - start_time;
		RS_INFO("%s : %lld ms", message_.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time).count());
	}

}