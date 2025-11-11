#include "pch.h"
#include "ParticleSimulatorWidget.h"
#include "Resource/Widget/RSButtonWidget.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/Input/RSInputClass.h"
#include "Resource/Object/Camera/RSCamera.h"
#include <stdlib.h>
#include <iomanip>

#define USE_EVADING_WIDGET false
#define GIVE_ME_A_ROCK_AND_STONE false
#define PARTICLE_SIMULATOR_WIDGET false
#define CAMERA_CONTROLLER false

#define PARTICLE_SIMPLE_WIDGET true

namespace SJG
{

using RS_Application::mRSAPPLICATION;
using RS_Input::mRSINPUT;
using RS_Input::mRSKEYBIND;
using namespace RS_Widget;

std::wstring to_wstring_with_precision(float value, int precision = 2) {
	std::wstringstream wss;
	wss << std::fixed << std::setprecision(precision) << value;
	return wss.str();
}

ParticleSimulatorWidget::ParticleSimulatorWidget()
{
}

ParticleSimulatorWidget::~ParticleSimulatorWidget()
{
}

void ParticleSimulatorWidget::Initialize()
{
	auto resouce_manager = RSResourceManager::GetInstance();
	m_widget_manager = resouce_manager->GetWidgetManager();

#if CAMERA_CONTROLLER
	float widget_button_height = 0.9f;
	float widget_button_widget_start_position = -0.5f;
	float distance_widgets = 0.1f;
	unsigned int count_widgets = 8;
	std::string widget_names[8] =
	{ "camera_iso", "camera_top", "camera_down", "camera_front", "camera_back", "camera_left", "camera_right", "camera_projection" };
	unsigned int button_widget_id_start = 40;


	for (unsigned int i = 0; i < count_widgets; i++)
	{
		m_widget_manager->AddButtonWidgetID(glm::vec2(widget_button_widget_start_position + distance_widgets * i, widget_button_height), glm::vec2(15.0f, 15.0f), widget_names[i], button_widget_id_start + i);
		m_widget_id_list.push(button_widget_id_start + i);
	}

	// Right panel
	auto nothing_panel = m_widget_manager->AddWidget(glm::vec2(0.5f, 0.0f), glm::vec2(300.0f, 1024.0f), "nothing_panel");
	nothing_panel->SetColor(1.0f, 0.0f, 0.0f);
	nothing_panel->SetAlpha(0.5f);
	nothing_panel->SetVisibility(RSVisibility::NONE_HIT_INVISIBLE);
	m_widget_id_list.push(nothing_panel->GetWidgetID());
#endif

#if 0
	std::wstring information_text[9] =
	{
		L"T : GUI On / Off",
		L"Num 1234 : Selected Obj Trans ( None, Translate, Rotate, Scale )",
		L"F1 / End : Rolling Start / Stop",
		L"Insert : Rolling Auto On / Off",
		L"WASD : Move",
		L"Space : Jump",
		L"R : Reset",
		L"P : Simulation Pause",
		L"DONT BE LAZY"
	};

	for (unsigned int i = 0; i < 9; i++)
	{
		auto inform_text_widget = m_widget_manager->AddTextWidget(glm::vec2(-0.98f, 0.95f - 0.05f * i), 0.15f, information_text[i], RSTextStaticDynamic::STATIC);
		m_widget_id_list.push(inform_text_widget->GetWidgetID());
	}
	//m_widget_manager->AddTextWidgetID(glm::vec2(-0.98f, -0.9f), 0.15f, L"Mint Americano", RSTextStaticDynamic::STATIC);

	//m_widget_manager->AddTextWidgetID(glm::vec2(-0.98f, 0.95f - 0.05f * i), 0.15f, information_text[i], RS_Widget::RSTextStaticDynamic::STATIC);

#endif

#if 0
	std::wstring important_text[20] =
	{
			L"미안하다 이거 보여주려고 어그로끌었다.. 나루토 사스케 싸움수준 ㄹㅇ 실화냐? 진짜",
			L"세계관최강자들의 싸움이다.. 그찐따같던 나루토가 맞나? 진짜 나루토는 전설이다",
			L"진짜옛날에 맨날나루토봘는데 왕같은존재인 호카게 되서 세계최강 전설적인 영웅",
			L"이된나루토보면 진짜내가다 감격스럽고 나루토 노래부터 명장면까지 가슴울리는 ",
			L"장면들이 뇌리에 스치면서 가슴이 웅장해진다..그리고 극장판에 카카시앞에 운석 ",
			L"날라오는 거대한 걸 사스케가 갑자기 순식간에 나타나서 부숴버리곤 개간지나게",
			L"나루토가 없다면 마을을 지킬 자는 나밖에 없다 라며 바람처럼 사라진장면은 진짜",
			L" 나루토처음부터 본사람이면 안울수가 없더라 진짜 너무 감격스럽고 보루토를 최",
			L"근에 알았는데  미안하다.. 지금20화보는데 진짜 나루토세대나와서 너무 감격스럽",
			L"고 모두어엿하게 큰거보니 내가 다 뭔가 알수없는 추억이라해야되나 그런감정이 이상",
			L"하게 얽혀있다.. 시노는 말이많아진거같다 좋은선생이고..그리고 보루토왜욕하냐 귀여",
			L"운데 나루토를보는것같다 성격도 닮았어 그리고버루토에 나루토사스케 둘이싸워도 이",
			L"기는 신같은존재 나온다는게 사실임?? 그리고인터닛에 쳐봣는디 이거 ㄹㅇㄹㅇ 진짜",
			L"팩트냐?? 저적이 보루토에 나오는 신급괴물임? 나루토사스케 합체한거봐라 진짜 ",
			L" 이거보고 개충격먹어가지고 와 소리 저절로 나오더라 ;; 진짜 저건 개오지는데.. 저",
			L"게 ㄹㅇ이면 진짜 꼭봐야돼 진짜 세계도 파괴시키는거아니야 .. 와 진짜 나루토사스케",
			L"가 저렇게 되다니 진짜 눈물나려고했다.. 버루토그라서 계속보는중인데 저거 ㄹㅇ이냐..?",
			L" 하.. 사스케 보고싶다..  진짜언제 이렇게 신급 최강들이 되었을까 옛날생각나고",
			L" 나 중딩때생각나고 뭔가 슬프기도하고 좋기도하고 감격도하고 여러가지감정이 복잡하",
			L"네.. 아무튼 나루토는 진짜 애니중최거명작임..",
	};

	for (unsigned int i = 0; i < 20; i++)
	{
		m_widget_manager->AddTextWidgetID(glm::vec2(-0.98f, 0.95f - 0.1f * i), 0.27f, important_text[i], 400 + i);
    m_widget_id_list.push(400 + i);
	}

#endif
	if (USE_EVADING_WIDGET)
	{
		// Create Widget that evade the mouse
		auto evading_panel = m_widget_manager->AddButtonWidget(glm::vec2(0.0f, 0.0f), glm::vec2(30.0f, 30.0f), "mouse_evading_panel");
		m_widget_id_list.push(evading_panel->GetWidgetID());
	}
	if (GIVE_ME_A_ROCK_AND_STONE)
	{
		auto rs_text_widget = m_widget_manager->AddTextWidget(glm::vec2(-0.98f, -0.9f), 0.15f, L"Rock & Stone", RSTextStaticDynamic::STATIC);
		m_widget_id_list.push(rs_text_widget->GetWidgetID());
		unsigned int Rns_icon_id = resouce_manager->GetTextureManager()->GetTexture_UINT("rns_icon");
		auto rs_icon_widget = m_widget_manager->AddWidget(glm::vec2(-0.72f, -0.9f), glm::vec2(15.0f, 15.0f), "rns_icon", Rns_icon_id);
		m_widget_id_list.push(rs_icon_widget->GetWidgetID());

	}

#if PARTICLE_SIMULATOR_WIDGET
	RS_Particle::RSParticleSimulator* p_PS = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
	RSParticleProperty PS_property = p_PS->GetParticleProperty();
	// Property widget
	unsigned int property_widget_id_start = 300;

	std::wstring current_render_property_min = L"";
	std::wstring current_render_property_max = L"";

	switch (p_PS->GetRenderPropertyType())
	{
	case RSRenderPropertyType::R_COLOR:
		current_render_property_min = L"Color";
		current_render_property_max = L"Color";
		break;

	case RSRenderPropertyType::R_VELOCITY:
		current_render_property_min = L"Velocity min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
		current_render_property_max = L"Velocity max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
		break;

	case RSRenderPropertyType::R_ACCELERATION:
		current_render_property_min = L"Acceleration min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
		current_render_property_max = L"Acceleration max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
		break;

	case RSRenderPropertyType::R_DENSITY:
		current_render_property_min = L"Density min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
		current_render_property_max = L"Density max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
		break;

	case RSRenderPropertyType::R_PRESSURE:
		current_render_property_min = L"Pressure min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
		current_render_property_max = L"Pressure max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
		break;

	}



	std::wstring particle_property_text[10] =
	{
		L"Particle Count : "			+ to_wstring_with_precision(static_cast<float>(p_PS->GetParticleCount()), 0),
		L"Particle Radius : "			+ to_wstring_with_precision(PS_property.particle_radius),
		L"Density : "					+ to_wstring_with_precision(PS_property.density),
		L"Viscosity : "					+ to_wstring_with_precision(PS_property.viscosity),
		L"Gravity : "					+ to_wstring_with_precision(PS_property.gravity),
		L"Mass : "						+ to_wstring_with_precision(PS_property.mass),
		L"Gas Constant : "				+ to_wstring_with_precision(PS_property.gas_constant),
		L"Smoothing Length : "			+ to_wstring_with_precision(PS_property.smoothing_length),
		current_render_property_max,
		current_render_property_min
	};

	std::string particle_property_text_name[10] =
	{
		"particle_count", "particle_radius", "density", "viscosity", "gravity", "mass", "gas_constant", "smoothing_length", "min_value", "max_value"
	};

	for (unsigned int i = 0; i < 10; i++)
	{
		auto particle_property_text_widget = m_widget_manager->AddTextWidgetID(glm::vec2(0.4f, 0.95f - 0.05f * i), 0.15f, particle_property_text[i], 300+i, RSTextStaticDynamic::STATIC);
		particle_property_text_widget->SetWidgetName(particle_property_text_name[i]);
		m_widget_id_list.push(particle_property_text_widget->GetWidgetID());
	}

	if (p_PS->GetSolverType() == RSSolverType::R_DYNAMIC_BRUTE_FORCE)
	{
    std::wstring current_particle_counter = L"Current Particle Count :" + to_wstring_with_precision(static_cast<float>(p_PS->GetInflowData()->current_count), 0);
    auto particle_counter_text_widget = m_widget_manager->AddTextWidgetID(glm::vec2(0.4f, 0.95f - 0.05f * 10), 0.15f, current_particle_counter, 310, RSTextStaticDynamic::DYNAMIC);
    m_widget_id_list.push(particle_counter_text_widget->GetWidgetID());
	}

	// Reset Button
	RSButtonWidget* restart_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.45f, 0.4f), glm::vec2(25.0f, 15.0f), "restart_btn", 350);
	restart_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
	restart_btn->SetCallback(std::bind(&ParticleSimulatorWidget::RestartButton, this));
	m_widget_id_list.push(restart_btn->GetWidgetID());

	// Property Update button
	RSButtonWidget* update_property_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.6f, 0.4f), glm::vec2(25.0f, 15.0f), "update_property_btn", 351);
	update_property_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
	update_property_btn->SetCallback(std::bind(&ParticleSimulatorWidget::UpdateSimulatorProperty, this));
	m_widget_id_list.push(update_property_btn->GetWidgetID());

	// Property Next button
	RSButtonWidget* next_legend_property_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.75f, 0.4f), glm::vec2(25.0f, 15.0f), "update_property_btn", 352);
	next_legend_property_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
	next_legend_property_btn->SetCallback(std::bind(&ParticleSimulatorWidget::NextLegendProperty, this));
	m_widget_id_list.push(next_legend_property_btn->GetWidgetID());
#endif

#if PARTICLE_SIMPLE_WIDGET
	RS_Particle::RSParticleSimulator* p_PS = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
	RSParticleProperty PS_property = p_PS->GetParticleProperty();
	// Property widget
	unsigned int property_widget_id_start = 300;

	std::wstring particle_property_text[1] =
	{
		L"Particle Count : " + to_wstring_with_precision(static_cast<float>(p_PS->GetParticleCount()), 0),
	};

	std::string particle_property_text_name[1] =
	{
		"particle_count"
	};

	for (unsigned int i = 0; i < 1; i++)
	{
		auto particle_property_text_widget = m_widget_manager->AddTextWidgetID(glm::vec2(0.4f, 0.95f - 0.05f * i), 0.15f, particle_property_text[i], property_widget_id_start + i, RSTextStaticDynamic::STATIC);
		particle_property_text_widget->SetWidgetName(particle_property_text_name[i]);
		particle_property_text_widget->SetTextColor(glm::vec3(1.0f));
		m_widget_id_list.push(particle_property_text_widget->GetWidgetID());
	}

	//// Reset Button
	//RSButtonWidget* restart_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.45f, 0.4f), glm::vec2(25.0f, 15.0f), "restart_btn", 350);
	//restart_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
	//restart_btn->SetCallback(std::bind(&ParticleSimulatorWidget::RestartButton, this));
	//m_widget_id_list.push(restart_btn->GetWidgetID());

	//// Property Update button
	//RSButtonWidget* update_property_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.6f, 0.4f), glm::vec2(25.0f, 15.0f), "update_property_btn", 351);
	//update_property_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
	//update_property_btn->SetCallback(std::bind(&ParticleSimulatorWidget::UpdateSimulatorProperty, this));
	//m_widget_id_list.push(update_property_btn->GetWidgetID());

	//// Property Next button
	//RSButtonWidget* next_legend_property_btn = m_widget_manager->AddButtonWidgetID(glm::vec2(0.75f, 0.4f), glm::vec2(25.0f, 15.0f), "update_property_btn", 352);
	//next_legend_property_btn->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("jordy_alpha6")->GetTexture());
	//next_legend_property_btn->SetCallback(std::bind(&ParticleSimulatorWidget::NextLegendProperty, this));
	//m_widget_id_list.push(next_legend_property_btn->GetWidgetID());

#endif

#if CAMERA_CONTROLLER
	auto number2_widget = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(45));
	number2_widget->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("ddungi")->GetTexture());
	number2_widget->SetCallback(std::bind(&ParticleSimulatorWidget::ClickFunction, this));


	auto btn_widget_1 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(40));
	btn_widget_1->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraTopView, this));

	auto btn_widget_2 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(41));
	btn_widget_2->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraBottomView, this));

	auto btn_widget_3 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(42));
	btn_widget_3->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraFrontView, this));

	auto btn_widget_4 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(43));
	btn_widget_4->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraBackView, this));

	auto btn_widget_5 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(44));
	btn_widget_5->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraLeftView, this));

	auto btn_widget_6 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(45));
	btn_widget_6->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraRightView, this));

	auto btn_widget_7 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(46));
	btn_widget_7->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraISOView, this));

	auto btn_widget_8 = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget(47));
	btn_widget_8->SetCallback(std::bind(&ParticleSimulatorWidget::Func_CameraSwitchProjection, this));
#endif

	if (USE_EVADING_WIDGET)
	{
		auto mouse_evading_panel = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget("mouse_evading_panel"));
		mouse_evading_panel->SetCallback(std::bind(&ParticleSimulatorWidget::ClickFunction, this));
		m_widget_id_list.push(mouse_evading_panel->GetWidgetID());
	}

}

void ParticleSimulatorWidget::Update(float dt)
{
	if (USE_EVADING_WIDGET)
	{
		// the widget that avoid the mouse update the position to evade mouse position
		auto mouse_evading_panel = m_widget_manager->GetWidget("mouse_evading_panel");
		glm::vec2 mouse_pos = glm::vec2(mRSINPUT->GetMousePosition().x, mRSINPUT->GetMousePosition().y);
		// mouse_pos  convert to -1.0f ~ 1.0f from screen space
		mouse_pos.x = (mouse_pos.x / mRSAPPLICATION->GetWidth()) * 2.0f - 1.0f;
		mouse_pos.y = 1.0f - (mouse_pos.y / mRSAPPLICATION->GetHeight()) * 2.0f;
		glm::vec2 panel_pos = glm::vec2(mouse_evading_panel->GetPosition().x, mouse_evading_panel->GetPosition().y);
		glm::vec2 direction = panel_pos - mouse_pos;
		if (glm::length(direction) < 0.005f)
		{
			return;
		}

		// when the distance between the mouse and the panel is less than 0.1f, the panel does not move
		int random_number = rand() % 100;
		if (random_number < 2)
			if (glm::length(direction) < 0.1f)
			{
				// calculate the direction to evade the mouse
				direction = glm::normalize(direction) * 0.1f;
				panel_pos += direction;
				mouse_evading_panel->SetPosition(panel_pos.x, panel_pos.y);
			}
	}

#if PARTICLE_SIMULATOR_WIDGET
	// property widget update
	if (RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->GetTextShouldUpdate())
	{
		// Get Particle Property
		auto p_PS = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
		auto PS_property = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->GetParticleProperty();

		std::wstring current_render_property_min = L"";
		std::wstring current_render_property_max = L"";

		switch (p_PS->GetRenderPropertyType())
		{
		case RSRenderPropertyType::R_COLOR:
			current_render_property_min = L"Color";
			current_render_property_max = L"Color";
			break;

		case RSRenderPropertyType::R_VELOCITY:
			current_render_property_min = L"Velocity min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
			current_render_property_max = L"Velocity max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
			break;

		case RSRenderPropertyType::R_ACCELERATION:
			current_render_property_min = L"Acceleration min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
			current_render_property_max = L"Acceleration max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
			break;

		case RSRenderPropertyType::R_DENSITY:
			current_render_property_min = L"Density min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
			current_render_property_max = L"Density max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
			break;

		case RSRenderPropertyType::R_PRESSURE:
			current_render_property_min = L"Pressure min : " + to_wstring_with_precision(p_PS->GetMinValueLegend());
			current_render_property_max = L"Pressure max : " + to_wstring_with_precision(p_PS->GetMaxValueLegend());
			break;

		}


		std::wstring particle_property_text[10] =
		{
			L"Particle Count : "			+ to_wstring_with_precision(static_cast<float>(p_PS->GetParticleCount()), 0),
			L"Particle Radius : "			+ to_wstring_with_precision(PS_property.particle_radius),
			L"Density : "					+ to_wstring_with_precision(PS_property.density),
			L"Viscosity : "					+ to_wstring_with_precision(PS_property.viscosity),
			L"Gravity : "					+ to_wstring_with_precision(PS_property.gravity),
			L"Mass : "						+ to_wstring_with_precision(PS_property.mass),
			L"Gas Constant : "				+ to_wstring_with_precision(PS_property.gas_constant),
			L"Smoothing Length : "			+ to_wstring_with_precision(PS_property.smoothing_length),
			current_render_property_max,
			current_render_property_min
		};

		for (unsigned int i = 0; i < 10; i++)
		{
			auto particle_property_text_widget = static_cast<RSTextWidget*>(m_widget_manager->GetWidget(300 + i));
			particle_property_text_widget->SetText(particle_property_text[i]);
		}
		RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->SetTextShouldUpdate(false);
	}

  // Dynamic Text Update
  if (RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->GetSolverType() == RSSolverType::R_DYNAMIC_BRUTE_FORCE)
  {
    std::wstring current_particle_counter = L"Current Particle Count :" + to_wstring_with_precision(static_cast<float>(RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->GetInflowData()->current_count), 0);
    auto particle_counter_text_widget = dynamic_cast<RSTextWidget*>(m_widget_manager->GetWidget(310));
    particle_counter_text_widget->SetText(current_particle_counter);
  }
	
#endif

#if PARTICLE_SIMPLE_WIDGET

	// property widget update
	if (RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->GetTextShouldUpdate())
	{
		// Get Particle Property
		const auto p_ps = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();

    std::wstring particle_property_text[1] =
		{
			L"Particle Count : " + to_wstring_with_precision(static_cast<float>(p_ps->GetParticleCount()), 0),
		};

		for (unsigned int i = 0; i < 1; i++)
		{
			auto particle_property_text_widget = dynamic_cast<RSTextWidget*>(m_widget_manager->GetWidget(300 + i));
			particle_property_text_widget->SetText(particle_property_text[i]);
		}
		RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator()->SetTextShouldUpdate(false);
	}

#endif

}

void ParticleSimulatorWidget::Shutdown()
{
	while (!m_widget_id_list.empty())
	{
		m_widget_manager->RemoveWidget(m_widget_id_list.top());
		m_widget_id_list.pop();
	}
	RSResourceManager::GetInstance()->GetWidgetManager()->ResetLastWidgetId();
}

void ParticleSimulatorWidget::ClickFunction()
{
	std::cout << "Clicked" << std::endl;
	if (USE_EVADING_WIDGET)
	{
		// get random number
		int random_number = rand() % 100;

		if (random_number < 10)
		{
			auto mouse_evading_panel = static_cast<RSButtonWidget*>(m_widget_manager->GetWidget("mouse_evading_panel"));
			// change texture
			mouse_evading_panel->SetFuncTexture(RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("ddungi")->GetTexture());
			// change size
			mouse_evading_panel->SetPosition(0.0f, 0.0f);
			mouse_evading_panel->SetSize(500.0f, 500.0f);
		}
	}

}

void ParticleSimulatorWidget::Func_CameraTopView()
{
	RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::TOP);
}

void ParticleSimulatorWidget::Func_CameraFrontView()
{
	RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::FRONT);
}

void ParticleSimulatorWidget::Func_CameraRightView()
{
	RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::RIGHT);
}

void ParticleSimulatorWidget::Func_CameraBackView()
{
	RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::BACK);
}

void ParticleSimulatorWidget::Func_CameraLeftView()
{
	RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::LEFT);
}

void ParticleSimulatorWidget::Func_CameraBottomView()
{
	RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::BOTTOM);
}

void ParticleSimulatorWidget::Func_CameraISOView()
{
	RSResourceManager::GetInstance()->GetCamera()->SetCameraViewPoint(ViewPointType::ISO);
}

void ParticleSimulatorWidget::Func_CameraSwitchProjection()
{
	RSResourceManager::GetInstance()->GetCamera()->SwitchCameraMode();
}

void ParticleSimulatorWidget::UpdateSimulatorProperty()
{
	auto PS = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
	PS->CallCalculateMinMaxValue();
}

void ParticleSimulatorWidget::NextLegendProperty()
{
	auto PS = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
	PS->SetNextRenderPropertyType();
}

void ParticleSimulatorWidget::RestartButton()
{
	RS_State::mRSSTATE->Restart();
}



} // namespace SJG