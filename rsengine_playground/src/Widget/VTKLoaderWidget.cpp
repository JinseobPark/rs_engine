#include "pch.h"
#include "VTKLoaderWidget.h"
#include "Resource/Widget/RSTextButtonWidget.h"
#include "Resource/Widget/RSButtonWidget.h"
#include "Systems/Input/RSInputClass.h"

#define BTN_WIDTH 100.0f
#define BTN_HEIGHT 30.0f

#define BTN_INIT_X -0.8f
#define BTN_INIT_Y 0.8f
#define BTN_MARGIN 0.15f

#define PARTICLE_SIMPLE_WIDGET false
#define CAMERA_CONTROLLER false

namespace SJG
{
  using namespace RS_Widget;
  VTKLoaderWidget::VTKLoaderWidget()
  {
    m_widget_manager = nullptr;
  }
  VTKLoaderWidget::~VTKLoaderWidget()
  {
  }
  void VTKLoaderWidget::Initialize()
  {
    const auto resource_manager = RSResourceManager::GetInstance();
    auto texture_manager = resource_manager->GetTextureManager();
    m_widget_manager = resource_manager->GetWidgetManager();

    // Load Text Button
    {
      std::string btn_name[4] = { "load_file_btn","load_next_btn", "load_prev_btn", "load_first" };
      std::wstring btn_text[4] = { L"LOAD FILE" , L"LOAD NEXT", L"LOAD PREV", L"LOAD FIRST" };

      void (VTKLoaderWidget:: * btn_func[4])() = { &VTKLoaderWidget::LoadFileBtn, &VTKLoaderWidget::LoadNextVtkBtn, &VTKLoaderWidget::LoadPrevVtkBtn, &VTKLoaderWidget::LoadFirstVtkFile };

      for (int i = 0; i < 4; i++)
      {
        const auto btn = new RSTextButtonWidget(btn_text[i]);
        btn->SetWidgetName(btn_name[i]);
        btn->SetSize(BTN_WIDTH, BTN_HEIGHT);
        btn->SetPosition(BTN_INIT_X, BTN_INIT_Y - BTN_MARGIN * static_cast<float>(i));
        btn->SetDefaultTexture(texture_manager->GetTexture("default")->GetTexture());
        btn->SetTextSize(0.12f);
        btn->SetFuncTexture(texture_manager->GetTexture("default")->GetTexture());
        btn->SetTextAlignment(RSTextAlignment::MIDDLE);

        btn->SetCallback(std::bind(btn_func[i], this));
        btn->Initialize();
        m_widget_manager->AddTextButtonWidget(btn);
        m_widget_id_list.push(btn->GetWidgetID());
      }
    }

#if CAMERA_CONTROLLER

    // Camera Widget Btn
    {
      const float widget_button_height = 0.9f;
      const float widget_button_widget_start_position = -0.5f;
      const float distance_widgets = 0.1f;
      const unsigned int count_widgets = 8;
      const std::string widget_names[8] =
      { "camera_iso", "camera_top", "camera_down", "camera_front", "camera_back", "camera_left", "camera_right", "camera_projection" };
      const unsigned int button_widget_id_start = 40;


      for (unsigned int i = 0; i < count_widgets; i++)
      {
        m_widget_manager->AddButtonWidgetID(glm::vec2(widget_button_widget_start_position + distance_widgets * i, widget_button_height), glm::vec2(15.0f, 15.0f), widget_names[i], button_widget_id_start + i);
        m_widget_id_list.push(button_widget_id_start + i);
      }


      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(40))->SetCallback([this] { FuncCameraTopView(); });
      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(41))->SetCallback([this] { FuncCameraBottomView(); });
      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(42))->SetCallback([this] { FuncCameraFrontView(); });
      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(43))->SetCallback([this] { FuncCameraBackView(); });
      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(44))->SetCallback([this] { FuncCameraLeftView(); });
      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(45))->SetCallback([this] { FuncCameraRightView(); });
      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(46))->SetCallback([this] { FuncCameraIsoView(); });
      dynamic_cast<RSButtonWidget*>(m_widget_manager->GetWidget(47))->SetCallback([this] { FuncCameraSwitchProjection(); });

    }
#endif

#if PARTICLE_SIMPLE_WIDGET
    {
      // Update Attribute Button
      RSButtonWidget* update_attrib_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.45f, 0.4f), glm::vec2(25.0f, 15.0f), "update_attrib_btn", 350);
      update_attrib_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
      update_attrib_btn->SetCallback([this] { UpdateAttributeData(); });
      m_widget_id_list.push(update_attrib_btn->GetWidgetID());

      // Next Attribute button
      RSButtonWidget* next_attrib_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.6f, 0.4f), glm::vec2(25.0f, 15.0f), "next_attrib_btn", 351);
      next_attrib_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
      next_attrib_btn->SetCallback([this] { NextAttributeIndex(); });
      m_widget_id_list.push(next_attrib_btn->GetWidgetID());

      // Prev Attribute button
      RSButtonWidget* prev_attrib_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.75f, 0.4f), glm::vec2(25.0f, 15.0f), "prev_attrib_btn", 352);
      prev_attrib_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
      prev_attrib_btn->SetCallback([this] { PrevAttributeIndex(); });
      m_widget_id_list.push(prev_attrib_btn->GetWidgetID());
    }
#endif

  }

  void VTKLoaderWidget::Update(float dt)
  {
  }

  void VTKLoaderWidget::Shutdown()
  {
    while (!m_widget_id_list.empty())
    {
      m_widget_manager->RemoveWidget(m_widget_id_list.top());
      m_widget_id_list.pop();
    }
    RSResourceManager::GetInstance()->GetWidgetManager()->ResetLastWidgetId();
  }

  void VTKLoaderWidget::LoadNextVtkBtn()
  {
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadNextVtkFile();
  }

  void VTKLoaderWidget::LoadPrevVtkBtn()
  {
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadPrevVtkFile();
  }

  void VTKLoaderWidget::FuncCameraTopView()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::TOP);
  }

  void VTKLoaderWidget::FuncCameraFrontView()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::FRONT);
  }

  void VTKLoaderWidget::FuncCameraRightView()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::RIGHT);
  }

  void VTKLoaderWidget::FuncCameraBackView()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::BACK);
  }

  void VTKLoaderWidget::FuncCameraLeftView()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::LEFT);
  }

  void VTKLoaderWidget::FuncCameraBottomView()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::BOTTOM);
  }

  void VTKLoaderWidget::FuncCameraIsoView()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::ISO);
  }

  void VTKLoaderWidget::FuncCameraSwitchProjection()
  {
    RSResourceManager::GetInstance()->GetCamera()->SwitchCameraMode();
  }

  void VTKLoaderWidget::UpdateAttributeData()
  {
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->AsyncMinMaxValueLegend();
  }

  void VTKLoaderWidget::NextAttributeIndex()
  {
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->SetNextAttributeData();
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->AsyncMinMaxValueLegend();
  }

  void VTKLoaderWidget::PrevAttributeIndex()
  {
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->SetPrevAttributeData();
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->AsyncMinMaxValueLegend();
  }

  void VTKLoaderWidget::LoadFirstVtkFile()
  {
    RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer()->LoadFirstVtkFile();
  }

  void VTKLoaderWidget::LoadFileBtn()
  {
    RSResourceManager::GetInstance()->GetFileIOManager()->TryFileIO();
  }
}

