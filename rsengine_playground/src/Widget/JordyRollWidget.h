/*****************************************************************//**
 * \file   JordyRollWidget.h
 * \brief  Jordy Roll widget
 * 
 * \author Jinseob Park
 * \date   19/02/2025
 *********************************************************************/
#ifndef JORDY_ROLL_WIDGET_H_
#define JORDY_ROLL_WIDGET_H_

#include "Resource/Manager/RSResourceManager.h"
#include <stack>

namespace SJG
{
  class JordyRollWidget
  {
  public:
    JordyRollWidget();
    ~JordyRollWidget();
    void Initialize();
    void Update(float dt);
    void Shutdown();

    void TestBtn1();
    void TestBtn2();

  private:
    RS_Widget::RSWidgetManager* m_widget_manager = nullptr;
    std::stack<unsigned int> m_widget_id_list;
  };
  
}

#endif
