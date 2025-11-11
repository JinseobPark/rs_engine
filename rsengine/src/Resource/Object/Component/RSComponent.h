
/******************************************************************************/
/*!
\file   RSComponent.h
\author Jinseob Park
\date   2024/08/01

This file contains component base class for object, 
	which includes type, owner, and initialize function.

*/
/******************************************************************************/
#ifndef RS_COMPONENT_H_
#define RS_COMPONENT_H_

#include "RSComponentType.h"
#include <string>

/**
 * @brief Object namespace
 */
namespace RS_Object
{
	class RSObject;
}

/**
 * @brief Component namespace for object components
 */
namespace RS_Component
{
	/**
	 * @brief Component base class for object
	 */
	class RSComponent
	{
	public:
		explicit RSComponent(const RSComponentType type_) : m_type(type_) {}

    virtual ~RSComponent() = default;
		RSComponent() = delete;

    virtual RSComponent* Clone() const = 0;

		/**
		 * @brief Initialize function for component
		 */
		virtual void Initialize() = 0;

		/**
		 * @brief Get name of component.
		 * This is virtual function, so it should be implemented in derived class.
		 * @return name of component
		 */
		virtual std::string GetName() = 0;

		/**
		 * @brief Set owner object
		 * @param owner_ owner object
		 */
		void SetOwner(RS_Object::RSObject* owner_) { p_owner = owner_; }
		/**
		 * @brief Get owner object
		 * @return owner object (RSObject)
		 */
		RS_N_D RS_Object::RSObject* GetOwner() const { return p_owner; }

		/**
		 * @brief Get type of component
		 * @return type of component
		 */
		RS_N_D RSComponentType GetType() const { return m_type; }



	private:
    RS_Object::RSObject* p_owner = nullptr; ///< Owner object
    RSComponentType m_type; ///< Type of component
	};
}

#endif // !RS_COMPONENT_H_