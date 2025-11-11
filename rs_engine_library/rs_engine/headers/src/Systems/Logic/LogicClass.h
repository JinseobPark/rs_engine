#pragma once
#include "Systems/RSSystemComponent.h"
#include "LogicDefine.h"
#include "Resource/RSImguiHandler.h"
#include "WidgetLogic.h"


#ifdef DEF_PL_ROLLING
#include "PL_Rolling.h"
#endif

namespace SPH_MINT
{
	enum class PL_TYPE
	{
		PL_NONE = 0,
		PL_ROLLING,
		PL_SNIPER
	};


	class LogicClass : public RSSystemComponent
	{
	public:
		LogicClass();
		~LogicClass();

		void Initialize() override;
		void PostInitialize() override;

		void Update(float) override;
		void Shutdown() override;

#ifdef DEF_PL_ROLLING
		PL_Rolling* GetPLRolling() { return &m_pl_rolling; }
#endif

		RS_Widget::WidgetLogic* GetWidgetLogic() { return m_widget_logic; }


		//void SetGizmoState();

		void PLSwitch(PL_TYPE pl_type);

	private:
#ifdef DEF_PL_ROLLING
		PL_Rolling m_pl_rolling;
#endif
		RS_Widget::WidgetLogic *m_widget_logic = nullptr;

		PL_TYPE m_pl_type = PL_TYPE::PL_NONE;
	};
	extern LogicClass* mLOGIC;
}

