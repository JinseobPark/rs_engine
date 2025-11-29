#include "pch.h"
#include "PointClipperData.h"
#include "GameData.h"

namespace SJG
{
  PointClipperData::PointClipperData()
  {
    m_resource_manager = RSResourceManager::GetInstance();
  }

  PointClipperData::~PointClipperData()
  {
  }

  void PointClipperData::Initialize()
  {
    m_resource_manager->GetGraphicsData()->clear_color[0] = 0.2890625f;
    m_resource_manager->GetGraphicsData()->clear_color[1] = 0.2890625f;
    m_resource_manager->GetGraphicsData()->clear_color[2] = 0.2890625f;
    //RS_Logger::mLOGGER->SetLogLevel(RS_LOG_LEVEL_ALL);
  }
  void PointClipperData::Shutdown()
  {
  }
  void PointClipperData::Update(float dt)
  {
    KeyUpdate(dt);
  }
  void PointClipperData::KeyUpdate(float dt)
  {

  }
  void PointClipperData::LoadBinFile(const char* file_path)
  {
  }
}


