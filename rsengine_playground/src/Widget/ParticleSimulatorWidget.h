#pragma once
#include "Resource/Manager/RSResourceManager.h"
#include <stack>


namespace SJG
{
class ParticleSimulatorWidget
{
public:
	ParticleSimulatorWidget();
	~ParticleSimulatorWidget();

	void Initialize();
	void Update(float dt);
	void Shutdown();


	void ClickFunction();

	void Func_CameraTopView();
	void Func_CameraFrontView();
	void Func_CameraRightView();
	void Func_CameraBackView();
	void Func_CameraLeftView();
	void Func_CameraBottomView();
	void Func_CameraISOView();
	void Func_CameraSwitchProjection();

	void UpdateSimulatorProperty();
	void NextLegendProperty();
	void RestartButton();

	//void Func_CameraView(ViewPointType type_);
private:
	RS_Widget::RSWidgetManager* m_widget_manager = nullptr;
	std::stack<unsigned int> m_widget_id_list;

};

}