/**********************************************************************
 * \file   VTKLoaderWidget.h
 * \brief  VTK Loader Widget
 *
 * \author Jinseob Park
 * \date   24/12/2024
 *********************************************************************/
#ifndef VTK_LOADER_WIDGET_H_
#define VTK_LOADER_WIDGET_H_

#include "Resource/Manager/RSResourceManager.h"
#include <stack>

namespace SJG
{
  class VTKLoaderWidget
  {
  public:
    VTKLoaderWidget();
    ~VTKLoaderWidget();
    void Initialize();
    void Update(float dt);
    void Shutdown();


    /**
     * @brief Load Next VTK Button
     */
    void LoadNextVtkBtn();


    /**
     * @brief Load Prev VTK Button
     */
    void LoadPrevVtkBtn();


    void FuncCameraTopView();
    void FuncCameraFrontView();
    void FuncCameraRightView();
    void FuncCameraBackView();
    void FuncCameraLeftView();
    void FuncCameraBottomView();
    void FuncCameraIsoView();
    void FuncCameraSwitchProjection();

    void UpdateAttributeData();
    void NextAttributeIndex();
    void PrevAttributeIndex();

    // Load First vtk file
    void LoadFirstVtkFile();

    void LoadFileBtn();




  private:
    RS_Widget::RSWidgetManager* m_widget_manager = nullptr;
    std::stack<unsigned int> m_widget_id_list;
  };
}



#endif