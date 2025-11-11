/******************************************************************************/
/*!
\file   RSEventItem.h
\author Jinseob Park
\date   2024/08/01

This file contains event items for event handler.

*/
/******************************************************************************/
#ifndef RS_EVENT_ITEM_H_
#define RS_EVENT_ITEM_H_

#include <glm/glm.hpp>
#include <string>

/**
 * @brief Event namespace. This namespace items will be removed at game engine.
 */
namespace RS_Event
{
	/**
	 * @brief Event type enum class.
	 */
	enum class EventType : std::uint8_t
	{
		NONE = 0,
		STL_FILE_OPEN,
		OBJ_FILE_OPEN,
		CAMERA_RESET,
		CAMERA_ROTATION_SENSITIVITY,
		OBJECT_ADD_MODEL_COMPONENT,
		OBJECT_ADD_RIGIDBODY_COMPONENT,
		OBJECT_MOVE,
		CREATE_OBJECT,
		DELETE_OBJECT,
	};

	/**
	 * @brief Event data union.
	 */
	union EventData
	{
		glm::vec3 vec3_ = glm::vec3(0.0f);
		double double_;
		float float_;
		int int_;
		bool bool_;
		short short_;

		EventData() {}

    explicit EventData(glm::vec3 input_) : vec3_(input_) {}

    explicit EventData(double input_) : double_(input_) {}

    explicit EventData(float input_) : float_(input_) {}

    explicit EventData(bool input_) : bool_(input_) {}

    explicit EventData(short input_) : short_(input_) {}

    explicit EventData(int input_) : int_(input_) {}

		~EventData() {}

	};

	struct RSEventItem
	{
    EventType type = EventType::NONE; ///< Event type
    EventData data; ///< Event data
    std::string name; ///< Event name
    std::string path; ///< Event path

		RSEventItem() = default; 
		~RSEventItem() = default;
	};
} // namespace SPH_MINT

#endif // RS_EVENT_ITEM_H_