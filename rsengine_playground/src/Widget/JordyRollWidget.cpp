#include "pch.h"
#include "JordyRollWidget.h"
#include "Resource/Widget/RSTextButtonWidget.h"
#include "Systems/Input/RSInputClass.h"

#define BTN_WIDTH 120.0f
#define BTN_HEIGHT 30.0f

#define BTN_INIT_X -0.7f
#define BTN_INIT_Y 0.8f
#define BTN_MARGIN 0.15f

namespace SJG
{
  JordyRollWidget::JordyRollWidget()
  {
    m_widget_manager = nullptr;
  }
  JordyRollWidget::~JordyRollWidget()
  {
  }
  void JordyRollWidget::Initialize()
  {
    const auto resource_manager = RSResourceManager::GetInstance();
    auto texture_manager = resource_manager->GetTextureManager();
    m_widget_manager = resource_manager->GetWidgetManager();
    // Load Text Button
    {
      std::string btn_name[2] = { "first button","Second Btn" };
      std::wstring btn_text[2] = { L"FIRST BUTTON" , L"Hangul" };
      void (JordyRollWidget:: * btn_func[2])() = { &JordyRollWidget::TestBtn1, &JordyRollWidget::TestBtn2 };
      for (int i = 0; i < 2; i++)
      {
        const auto btn = new RS_Widget::RSTextButtonWidget(btn_text[i]);
        btn->SetSize(BTN_WIDTH, BTN_HEIGHT);
        btn->SetPosition(BTN_INIT_X, BTN_INIT_Y - BTN_MARGIN * static_cast<float>(i));
        btn->SetDefaultTexture(texture_manager->GetTexture("default")->GetTexture());
        btn->SetTextSize(0.12f);
        btn->SetFuncTexture(texture_manager->GetTexture("bok")->GetTexture());
        btn->SetTextAlignment(RSTextAlignment::MIDDLE);
        btn->SetCallback(std::bind(btn_func[i], this));
        btn->Initialize();
        m_widget_manager->AddTextButtonWidget(btn);
        m_widget_id_list.push(btn->GetWidgetID());
      }
    }
  }

  void JordyRollWidget::Update(float dt)
  {
  }

  void JordyRollWidget::Shutdown()
  {
    while (!m_widget_id_list.empty())
    {
      m_widget_manager->RemoveWidget(m_widget_id_list.top());
      m_widget_id_list.pop();
    }
    RSResourceManager::GetInstance()->GetWidgetManager()->ResetLastWidgetId();
  }

  void JordyRollWidget::TestBtn1()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::TOP);
  }

  void JordyRollWidget::TestBtn2()
  {
    RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::RIGHT);
  }
}
