#include "pch.h"
#include "VTKLoaderData.h"
#include "GameData.h"

namespace SJG
{
  VTKLoaderData::VTKLoaderData()
  {
    m_resource_manager = RSResourceManager::GetInstance();
    m_legend_widget = nullptr;
  }

  VTKLoaderData::~VTKLoaderData()
  {
  }

  void VTKLoaderData::Initialize()
  {
    m_resource_manager->GetGraphicsData()->clear_color[0] = 0.84f;
    m_resource_manager->GetGraphicsData()->clear_color[1] = 0.6523f;
    m_resource_manager->GetGraphicsData()->clear_color[2] = 1.0f;

    if (m_legend_widget == nullptr)
    {
      m_legend_widget = new RS_Widget::RSLegendWidget();
      m_legend_widget->Initialize();
      // The Legend widget is clickable.When dragging the legend widget, the legend widget is moved.

      m_legend_widget->SetPosition(0.9f, 0.75f);
      m_legend_widget->SetSize(20.0f, 120.0f);
      m_legend_widget->SetVertical(true);
      m_legend_widget->SetWidgetName(LEGEND_WIDGET_NAME);
      m_resource_manager->GetWidgetManager()->AddButtonWidgetID(m_legend_widget, 60);
    }
  }

  void VTKLoaderData::Shutdown()
  {
    if (m_legend_widget != nullptr)
      m_resource_manager->GetWidgetManager()->RemoveWidget(m_legend_widget->GetWidgetID());
    m_legend_widget = nullptr;
  }

  void VTKLoaderData::Update(float dt)
  {
    KeyUpdate(dt);
  }

  void VTKLoaderData::KeyUpdate(float dt)
  {

    if (RS_Input::mRSINPUT->IsTriggered(RSKey::Q))
    {
      RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadPrevVtkFile();
    }
    if (RS_Input::mRSINPUT->IsPressed(RSKey::A))
    {
      RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadPrevVtkFile();
    }
    if (RS_Input::mRSINPUT->IsTriggered(RSKey::E))
    {
      RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadNextVtkFile();
    }
    if (RS_Input::mRSINPUT->IsPressed(RSKey::S))
    {
      RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadNextVtkFile();
    }

    if (RS_Input::mRSINPUT->IsTriggered(RSKey::B))
    {
      m_legend_widget->SetDividedValue(0);
      RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->SetLegendTextureId(m_legend_widget->GetOriginTexture());
    }
    if (RS_Input::mRSINPUT->IsTriggered(RSKey::N))
    {
      m_legend_widget->SetDividedValue(m_legend_widget->GetDividedValue() + 1);
      RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->SetLegendTextureId(m_legend_widget->GetDividedTexture());
    }
    if (RS_Input::mRSINPUT->IsTriggered(RSKey::M))
    {
      m_legend_widget->SetVertical(!m_legend_widget->GetVertical());
    }
  }

  void VTKLoaderData::LoadVTKFile(const char* file_path)
  {
  }
}
