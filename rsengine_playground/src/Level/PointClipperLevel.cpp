#include "pch.h"
#include "PointClipperLevel.h"
#include "../LevelData/PointClipperData.h"
#include "../Widget/PointClipperWidget.h"
#include "Systems/Application/RSApplication.h"
#include "GameData.h"

namespace SJG
{
  PointClipperData* m_point_clipper_data = nullptr;
  PointClipperWidget* m_point_clipper_widget = nullptr;

  void PointClipperLoad()
  {
    m_resource_manager = RSResourceManager::GetInstance();
    m_resource_manager->GetParticleManager()->CreatePointClipper();
    m_point_clipper_data = new PointClipperData();
    m_point_clipper_widget = new PointClipperWidget();
    // Get application instance

    // Create glfw image 2x2
    RS_Application::mRSAPPLICATION->SetWindowIcon("resources/InitLoadFiles/Textures/rns_icon.png");
    RS_Application::mRSAPPLICATION->SetWindowTitle("Point Cloud Viewer");
  }

  void PointClipperInitialize()
  {
    m_point_clipper_data->Initialize();

    m_point_clipper_widget->Initialize();
  }

  void PointClipperUpdate(float dt)
  {
    m_point_clipper_data->Update(dt);
    m_point_clipper_widget->Update(dt);
    GameData::GetInstance()->Update(dt);
  }

  void PointClipperShutdown()
  {
    m_point_clipper_data->Shutdown();
    m_point_clipper_widget->Shutdown();
  }

  void PointClipperUnload()
  {
    m_resource_manager->GetParticleManager()->ShutdownPointClipper();
    delete m_point_clipper_data;
    m_point_clipper_data = nullptr;

    delete m_point_clipper_widget;
    m_point_clipper_widget = nullptr;

  }



}