/**********************************************************************
 * \file   PointClipperWidget.h
 * \brief  Point Clipper Widget
 * 
 * \author Jinseob Park
 * \date   06/11/2024
 *********************************************************************/
#ifndef POINT_CLIPPER_WIDGET_H_
#define POINT_CLIPPER_WIDGET_H_
#include "Resource/Manager/RSResourceManager.h"
#include <stack>

namespace SJG
{
  class PointClipperWidget
  {
  public:
    PointClipperWidget();
    ~PointClipperWidget();

    void Initialize();
    void Update(float dt);
    void Shutdown();

    void LoadBtn();
    void SaveBtn();
    void ViewBtn();
    void TranslationBtn();
    void RotationBtn();
    void ScaleBtn();

  private:
    RS_Widget::RSWidgetManager* m_widget_manager = nullptr;
    std::stack<unsigned int> m_widget_id_list;

  };

}

#endif

