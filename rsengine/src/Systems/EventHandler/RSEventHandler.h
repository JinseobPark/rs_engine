/******************************************************************************/
/*!
\file   RSEventHandler.h
\author Jinseob Park
\date   2024/08/01

This file contains event handler class for Game engine and logic part.
This file will be removed at game engine.
*/
/******************************************************************************/
#ifndef RS_EVENT_HANDLER_H_
#define RS_EVENT_HANDLER_H_

#define USING_IMGUI 1

#include "pch.h"
#include "Systems/RSSystemHeader.h"
#include "Systems/RSSystemComponent.h"
#include "RSEventItem.h"
#include "Resource/RSImguiHandler.h"
#include <queue>


/**
 * @brief Event namespace. This namespace items will be removed at game engine.
 */
namespace RS_Event
{
	/**
	 * @brief Event handler class.
	 * This class derived from System Component.
	 */
	class RSEventHandler : public RSSystemComponent
	{
	public:
		RSEventHandler();
		~RSEventHandler() override;

		void Initialize() override;
		void Update(float dt) override;
		void UpdatePost(float dt) const;
		void Shutdown() override;

		// Add Event
		void AddEvent(const RSEventItem& event_);
		void AddEvent(const EventType type_, const std::string& name_, const EventData& data_);
		void AddEvent(const EventType type_, const std::string& name_, const std::string& path_);
		void AddEvent(const EventType type_, const std::string& name_);
		void AddEvent(const EventType type_, const EventData& data_);
		void AddEvent(const EventType type_);


		// Make function to is included the event type
		bool IsIncludeEvent(const EventType type_, const std::vector<EventType>& list_);

		bool GetIsImguiShow() const { return b_is_imgui_show; }
		RS_Handler::RSImguiHandler* GetImguiHandler() const { return m_imgui_handler; }
		private:
      std::queue<RSEventItem> m_event_queue; ///< Event queue
      void ProcessEvent(const RSEventItem& event_); ///< Process event

      bool b_is_imgui_show = false; ///< Check imgui show or not

      RS_Handler::RSImguiHandler* m_imgui_handler; ///< Imgui handler

		// Check event type and add event are matched
		std::vector<EventType> event_no_data_list = {
			EventType::CAMERA_RESET,
		};

		std::vector<EventType> event_data_list = {
			EventType::CAMERA_ROTATION_SENSITIVITY,
		};

		std::vector<EventType> event_name_data_list = {
			EventType::OBJECT_MOVE,
		};

		std::vector<EventType> event_name_path_list = {
			EventType::STL_FILE_OPEN,
			EventType::OBJ_FILE_OPEN,
		};

		std::vector<EventType> event_name_list = {
			EventType::OBJECT_ADD_MODEL_COMPONENT,
			EventType::OBJECT_ADD_RIGIDBODY_COMPONENT,
			EventType::CREATE_OBJECT,
			EventType::DELETE_OBJECT,
		};
	};
	extern RSEventHandler* mRSEVENTHANDLER;
}

#endif // RS_EVENT_HANDLER_H_