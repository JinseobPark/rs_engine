#pragma once
#include "Resource/Manager/RSResourceManager.h"

//enum class ViewPointType;

namespace RS_Widget
{
	class WidgetLogic
	{
	public:
		WidgetLogic();
		~WidgetLogic();

		void Initialize();
		void PostInitialize();
		void Update(float dt);
		void Draw();
		void Shutdown();

		void SetGizmoState();


		void ClickFunction();

		void Func_CameraTopView();
		void Func_CameraFrontView();
		void Func_CameraRightView();
		void Func_CameraBackView();
		void Func_CameraLeftView();
		void Func_CameraBottomView();
		void Func_CameraISOView();
		void Func_CameraSwitchProjection();

		//void Func_CameraView(ViewPointType type_);
	private:
		RS_Widget::RSWidgetManager* m_widget_manager = nullptr;

	};
}