#include "pch.h"
#include "JordyRollLevel.h"
#include "../LevelData/JordyRollData.h"
#include "../Widget/JordyRollWidget.h"
#include "GameData.h"

namespace SJG
{
	JordyRollData* m_jordy_roll_data = nullptr;
  JordyRollWidget* m_jordy_roll_widget = nullptr;

	void JordyLoad()
	{
		m_resource_manager = RSResourceManager::GetInstance();
		m_jordy_roll_data = new JordyRollData();
		m_jordy_roll_widget = new JordyRollWidget();

		RS_Application::mRSAPPLICATION->SetWindowIcon("resources/InitLoadFiles/Textures/jordy_alpha2.png");
		RS_Application::mRSAPPLICATION->SetWindowTitle("Jordy kawaii");
	}

	void JordyInitialize()
	{
		m_jordy_roll_data->Initialize();
		m_jordy_roll_widget->Initialize();
	}

	void JordyUpdate(const float dt)
	{
		m_jordy_roll_data->Update(dt);

		//if (RS_Timer::mRSTIMER->getTotalTime() > 20.0f)
		//	RSResourceManager::GetInstance()->GetGraphicsData()->is_show_skybox = true;
		//else
			RSResourceManager::GetInstance()->GetGraphicsData()->skybox_timer_value -= 10.0f * dt;

		GameData::GetInstance()->Update(dt);
	}

	void JordyShutdown()
	{
		m_jordy_roll_data->Shutdown();
    m_jordy_roll_widget->Shutdown();
	}

	void JordyUnload()
	{
		delete m_jordy_roll_data;
		m_jordy_roll_data = nullptr;

    delete m_jordy_roll_widget;
    m_jordy_roll_widget = nullptr;
	}
}