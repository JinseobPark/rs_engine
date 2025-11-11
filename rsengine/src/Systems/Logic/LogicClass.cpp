#include "pch.h"
#include "LogicClass.h"
#include "Systems/Input/RSInputClass.h"
#include "Systems/Timer/RSTimer.h"
#include "Resource/Manager/RSResourceManager.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/constants.hpp"
#include "Resource/Object/Component/RSComponentHeader.h"
#include <iostream>

namespace SPH_MINT
{
	using namespace RS_Component;
	using namespace RS_Mesh;
	using RS_Input::mRSINPUT;
	using RS_Input::mRSKEYBIND;

	LogicClass* mLOGIC = nullptr;
	LogicClass::LogicClass()
	{
		assert(mLOGIC == nullptr && "Only One Logic Class");
		mLOGIC = this;
		m_widget_logic = new RS_Widget::WidgetLogic();
	}

	LogicClass::~LogicClass()
	{
		delete m_widget_logic;
	}

	void LogicClass::Initialize()
	{
		auto resource_manager = RSResourceManager::GetInstance();

		m_widget_logic->Initialize();
	}

	void LogicClass::PostInitialize()
	{
		auto resource_manager = RSResourceManager::GetInstance();

#ifdef DEF_PL_ROLLING
		m_pl_rolling.PostInitialize();
		m_pl_rolling.PlayInitializer();
#endif

		assert(m_widget_logic != nullptr && "Widget Logic is not constructed");
		m_widget_logic->PostInitialize();


		{ // macaron
			resource_manager->GetObjectManager()->CreateObject("WeLuvMint");
			auto maca = resource_manager->GetObjectManager()->GetObject("WeLuvMint");

			RSModel* maca_model = new RSModel();
			maca->AddComponent(maca_model);

			RSMesh* maca_mesh = new RSMesh();
			maca_mesh->SetMeshData(resource_manager->GetMeshManager()->GetMesh("macaron"));
			maca_mesh->SetTextureID(resource_manager->GetTextureManager()->GetTexture("mint_tex")->GetTexture());
			maca_mesh->SetIsTexture(true);

			maca_model->SetMesh(maca_mesh);
			maca->GetTransform()->AddPosition(glm::vec3(0.0f, 1.0f, 0.0f));

		}

		{
			// Plane RSObject
			resource_manager->GetObjectManager()->CreateObject("Grid");
			auto plane = resource_manager->GetObjectManager()->GetObject("Grid");

			RSModel* plane_model = new RSModel();
			plane->AddComponent(plane_model);

			RSMesh* plane_mesh = new RSMesh();
			plane_mesh->SetMeshData(resource_manager->GetMeshManager()->GetMesh("Grid"));
			plane_mesh->SetIsTexture(true);

			plane_model->SetMesh(plane_mesh);
			plane->GetTransform()->SetScale(20.0f);
		}

#if 0
		{
			// Stencil Plane RSObject
			resource_manager->GetObjectManager()->CreateObject("StencilGrid");
			auto stencil_grid = resource_manager->GetObjectManager()->GetObject("StencilGrid");

			RSModel* wtencil_grid_model = new RSModel();
			stencil_grid->AddComponent(wtencil_grid_model);

			RSMesh* stencil_grid_mesh = new RSMesh();
			stencil_grid_mesh->SetMeshData(resource_manager->GetMeshManager()->GetMesh("Grid"));
			stencil_grid_mesh->SetIsTexture(true);

			wtencil_grid_model->SetMesh(stencil_grid_mesh);
			stencil_grid->GetTransform()->SetScale(20.0f);
			stencil_grid->GetTransform()->SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
		}
#endif
	}

	void LogicClass::Update(float dt)
	{
#ifdef DEF_PL_ROLLING
		if(m_pl_rolling.IsPlay())
			m_pl_rolling.Update(dt);

		// And, when player moved, player should be rolled. the player is a sphere
		if (mRSINPUT->IsTriggered(mRSKEYBIND->GetKey(RSKeyList::PLAY_GAME_1)))
			PLSwitch(PL_TYPE::PL_ROLLING);
#endif


		assert(m_widget_logic != nullptr && "Widget Logic is not constructed");
		m_widget_logic->Update(dt);

		// RSGizmo

		if (RS_Timer::mRSTIMER->getTotalTime() > 20.0f)
			RSResourceManager::GetInstance()->GetGraphicsData()->is_show_skybox = true;

		RSResourceManager::GetInstance()->GetGraphicsData()->skybox_timer_value -= dt;

		if (mRSINPUT->IsTriggered(mRSKEYBIND->GetKey(RSKeyList::PLAY_GAME_OUT)))
		{
			PLSwitch(PL_TYPE::PL_NONE);
		}
	}


	void LogicClass::Shutdown()
	{
#ifdef DEF_PL_ROLLING
		m_pl_rolling.PlayShutdown();
#endif
		m_widget_logic->Shutdown();
	}

	void LogicClass::PLSwitch(PL_TYPE pl_type)
	{

#ifdef DEF_PL_ROLLING
		m_pl_rolling.PlayShutdown();
#endif

		switch (pl_type)
		{
			case PL_TYPE::PL_ROLLING:
				m_pl_rolling.PlayInitializer();
				break;
			case PL_TYPE::PL_SNIPER:
				break;

		}

	}
}
