#include "pch.h"
#include "RSEventHandler.h"
#include "Resource/Manager/RSResourceManager.h"
#include <iostream>

namespace RS_Event
{
	using namespace RS_Object;
	using RS_Input::mRSINPUT;
	using RS_Input::mRSKEYBIND;

	RSEventHandler* mRSEVENTHANDLER = nullptr;

	RSEventHandler::RSEventHandler()
	{
		assert(mRSEVENTHANDLER == nullptr && "Only One Event Handler");
		mRSEVENTHANDLER = this;
		m_imgui_handler = nullptr;
	}

	RSEventHandler::~RSEventHandler() = default;

	void RSEventHandler::Initialize()
	{
		m_imgui_handler = new RS_Handler::RSImguiHandler();
		m_imgui_handler->Initialize();
	}

	void RSEventHandler::Update(const float dt)
	{
#if USING_IMGUI
		if (mRSINPUT->IsTriggered(mRSKEYBIND->GetKey(RSKeyList::WIDGET_VIEW_TOGGLE)))
		{
			b_is_imgui_show = !b_is_imgui_show;
			//if(b_is_imgui_show)  RSResourceManager::GetInstance()->GetWidgetManager()->GetWidget("nothing_panel")->SetVisibility(RSVisibility::HIT_VISIBLE);
			//	else RSResourceManager::GetInstance()->GetWidgetManager()->GetWidget("nothing_panel")->SetVisibility(RSVisibility::NONE_HIT_INVISIBLE);
		}

		m_imgui_handler->UpdatePre(dt);
#endif
		while (!m_event_queue.empty())
		{
			const RSEventItem& event_item = m_event_queue.front();
			ProcessEvent(event_item);
			m_event_queue.pop();
		}
	}

	void RSEventHandler::UpdatePost(const float dt) const
	{
#if USING_IMGUI

		if (b_is_imgui_show)
			m_imgui_handler->UpdatePost(dt);
#endif
	}

	void RSEventHandler::Shutdown()
	{
		m_imgui_handler->Shutdown();
		delete m_imgui_handler;
		while (!m_event_queue.empty())
		{
			m_event_queue.pop();
		}
	}

	void RSEventHandler::AddEvent(const RSEventItem& event_)
	{
		m_event_queue.push(event_);
	}

	void RSEventHandler::AddEvent(const EventType type_, const std::string& name_, const EventData& data_)
	{
		if (!IsIncludeEvent(type_, event_name_data_list)) return;
		RSEventItem event_item;
		event_item.type = type_;
		event_item.name = name_;
		event_item.data = data_;
		m_event_queue.push(event_item);
	}

	void RSEventHandler::AddEvent(const EventType type_, const std::string& name_, const std::string& path_)
	{
		if (!IsIncludeEvent(type_, event_name_path_list)) return;
		RSEventItem event_item;
		event_item.type = type_;
		event_item.name = name_;
		event_item.path = path_;
		m_event_queue.push(event_item);
	}

	void RSEventHandler::AddEvent(const EventType type_, const std::string& name_)
	{
		if (!IsIncludeEvent(type_, event_name_list)) return;
		RSEventItem event_item;
		event_item.type = type_;
		event_item.name = name_;
		m_event_queue.push(event_item);
	}

	void RSEventHandler::AddEvent(const EventType type_, const EventData& data_)
	{
		if (!IsIncludeEvent(type_, event_data_list)) return;
		RSEventItem event_item;
		event_item.type = type_;
		event_item.data = data_;
		m_event_queue.push(event_item);
	}

	void RSEventHandler::AddEvent(const EventType type_)
	{
		if (!IsIncludeEvent(type_, event_no_data_list)) return;
		RSEventItem event_item;
		event_item.type = type_;
		m_event_queue.push(event_item);
	}

	bool RSEventHandler::IsIncludeEvent(const EventType type_, const std::vector<EventType>& list_)
	{
		for (const auto event_type : list_)
		{
			if (event_type == type_) return true;
		}
		return false;
	}

	void RSEventHandler::ProcessEvent(const RSEventItem& event_)
	{
		const RSResourceManager* resource_manager = RSResourceManager::GetInstance();
		RSObject* target_obj = nullptr;
		
		switch (event_.type)
		{
		case EventType::NONE:
			break;
		case EventType::STL_FILE_OPEN:
			RS_INFO("File Open : %s", event_.path.c_str());
			break;
		case EventType::OBJ_FILE_OPEN:
			RS_INFO("File Open : %s", event_.path.c_str());
			break;
		case EventType::CAMERA_RESET:
			resource_manager->GetCamera()->Reset();
			RS_INFO("Camera Reset");
			break;
		case EventType::CAMERA_ROTATION_SENSITIVITY:
			resource_manager->GetCamera()->SetRotationSensitivity(event_.data.float_);
			RS_INFO("Camera Rotation Sensitivity : %f", event_.data.float_);
			break;
		case EventType::OBJECT_ADD_MODEL_COMPONENT:
			target_obj = resource_manager->GetObjectManager()->GetObjectW(event_.name);
			if (target_obj == nullptr) return;
			if (target_obj->GetComponent(RSComponentType::CT_MODEL) == nullptr)
			{
				const auto model = new RSModel();
				target_obj->AddComponent(model);
			}
			RS_INFO("Add RSModel RSComponent : %s", event_.name.c_str());
			break;
		case EventType::OBJECT_ADD_RIGIDBODY_COMPONENT:
			target_obj = resource_manager->GetObjectManager()->GetObjectW(event_.name);
			if (target_obj == nullptr) return;
			if (target_obj->GetComponent(RSComponentType::CT_RIGIDBODY) == nullptr)
			{
				auto rigidbody = new RSRigidbody();
				target_obj->AddComponent(rigidbody);
			}
			RS_INFO("Add RSRigidbody RSComponent : %s", event_.name.c_str());
			break;
		case EventType::OBJECT_MOVE:
			target_obj = resource_manager->GetObjectManager()->GetObjectW(event_.name);
			if (target_obj == nullptr) return;
			target_obj->GetTransform()->AddPosition(event_.data.vec3_);
			RS_INFO("RSObject Move : %s", event_.name.c_str());
			break;
		case EventType::CREATE_OBJECT:
			target_obj = new RSObject();
			target_obj->SetName(event_.name);
			resource_manager->GetObjectManager()->AddObject(target_obj);
			RS_INFO("Create RSObject : %s", event_.name.c_str());
			break;
		case EventType::DELETE_OBJECT:
			target_obj = resource_manager->GetObjectManager()->GetObjectW(event_.name);
			if (target_obj == nullptr) return;
			resource_manager->GetObjectManager()->RemoveObject(target_obj);
			RS_INFO("Delete RSObject : %s", event_.name.c_str());
			break;

		}
	}
}
