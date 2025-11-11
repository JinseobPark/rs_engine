#include "pch.h"
#include "VTKLoaderLevel.h"
#include "../LevelData/VTKLoaderData.h"
#include "../Widget/VTKLoaderWidget.h"
#include "GameData.h"


namespace SJG
{
  VTKLoaderData* m_vtk_loader_data = nullptr;
  VTKLoaderWidget* m_vtk_loader_widget = nullptr;
  void VTKLoaderLoad()
  {
    m_resource_manager = RSResourceManager::GetInstance();
    m_resource_manager->GetParticleManager()->CreateVtkViewer();
    m_vtk_loader_data = new VTKLoaderData();
    m_vtk_loader_widget = new VTKLoaderWidget();

    RS_Application::mRSAPPLICATION->SetWindowIcon("resources/InitLoadFiles/Textures/snow.png");
    RS_Application::mRSAPPLICATION->SetWindowTitle("NFlow PostProcessor");
  }

  void VTKLoaderInitialize()
  {
    m_vtk_loader_data->Initialize();
    m_vtk_loader_widget->Initialize();
  }

  void VTKLoaderUpdate(const float dt)
  {
    m_vtk_loader_data->Update(dt);
    m_vtk_loader_widget->Update(dt);
    GameData::GetInstance()->Update(dt);
  }

  void VTKLoaderShutdown()
  {
    m_vtk_loader_data->Shutdown();
    m_vtk_loader_widget->Shutdown();
  }

  void VTKLoaderUnload()
  {
    // TODO : Shutdown all VTK Viewers without idx
    m_resource_manager->GetParticleManager()->ShutdownVtkViewer();
    delete m_vtk_loader_data;
    m_vtk_loader_data = nullptr;

    delete m_vtk_loader_widget;
    m_vtk_loader_widget = nullptr;
  }
}
