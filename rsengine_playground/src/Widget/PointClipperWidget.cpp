#include "pch.h"
#include "PointClipperWidget.h"
#include "Resource/Widget/RSButtonWidget.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/Input/RSInputClass.h"

#define BTN_WIDTH 80.0f
#define BTN_HEIGHT 30.0f

#define BTN_INIT_X -0.8f
#define BTN_INIT_Y 0.8f
#define BTN_MARGIN 0.15f

namespace SJG
{
  using namespace RS_Widget;
  PointClipperWidget::PointClipperWidget()
  {
    m_widget_manager = nullptr;
  }
  PointClipperWidget::~PointClipperWidget()
  {
  }
  void PointClipperWidget::Initialize()
  {
    const auto resource_manager = RSResourceManager::GetInstance();
    auto texture_manager = resource_manager->GetTextureManager();
    m_widget_manager = resource_manager->GetWidgetManager();

    std::string btn_name[6] = { "load_text", "save_text", "view_text", "translation_text", "rotation_text", "scale_text" };
    void (PointClipperWidget:: *btn_func[6])() = 
    { &PointClipperWidget::LoadBtn, &PointClipperWidget::SaveBtn, &PointClipperWidget::ViewBtn, 
      &PointClipperWidget::TranslationBtn, &PointClipperWidget::RotationBtn, &PointClipperWidget::ScaleBtn };

    for (int i = 0; i < 6; i++)
    {
      auto btn = new RSButtonWidget();
      btn->SetSize(BTN_WIDTH, BTN_HEIGHT);
      btn->SetPosition(BTN_INIT_X, BTN_INIT_Y - BTN_MARGIN * static_cast<float>(i));
      btn->SetDefaultTexture(texture_manager->GetTexture(btn_name[i])->GetTexture());
      btn->SetFuncTexture(texture_manager->GetTexture(btn_name[i])->GetTexture());
      btn->SetCallback(std::bind(btn_func[i], this));

      m_widget_manager->AddButtonWidget(btn);
      m_widget_id_list.push(btn->GetWidgetID());
    }

  }
  void PointClipperWidget::Update(float dt)
  {

  }
  void PointClipperWidget::Shutdown()
  {
    while (!m_widget_id_list.empty())
    {
      m_widget_manager->RemoveWidget(m_widget_id_list.top());
      m_widget_id_list.pop();
    }
    RSResourceManager::GetInstance()->GetWidgetManager()->ResetLastWidgetId();
  }
  void PointClipperWidget::LoadBtn()
  {
    RS_DEBUG("Load Button Clicked");
    // TODO : Load the points
    RSResourceManager::GetInstance()->GetFileIOManager()->TryFileIO();
  }
  void PointClipperWidget::SaveBtn()
  {
    RS_DEBUG("Save Button Clicked");
    // TODO : Save the clipped points
    RSResourceManager::GetInstance()->GetFileIOManager()->SaveFileDialog(L"bin");
  }
  void PointClipperWidget::ViewBtn()
  {
    RS_DEBUG("View Button Clicked");
    // TODO : Change View mode
    RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper()->SwitchClipping();
  }
  void PointClipperWidget::TranslationBtn()
  {
    RS_DEBUG("Translation Button Clicked");
    RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::TRANSLATION);
  }
  void PointClipperWidget::RotationBtn()
  {
    RS_DEBUG("Rotation Button Clicked");
    RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::ROTATION);
  }
  void PointClipperWidget::ScaleBtn()
  {
    RS_DEBUG("Scale Button Clicked");
    RSResourceManager::GetInstance()->GetWidgetManager()->GetGizmo()->SetState(RSGizmoState::SCALE);
  }
}
