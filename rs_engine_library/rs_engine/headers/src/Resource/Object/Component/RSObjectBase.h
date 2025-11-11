/******************************************************************************/
/*!
\file   RSObjectBase.h
\author Jinseob Park
\date   2024/08/01

This file contains object base class for object, 
	which includes object id and object name.

*/
/******************************************************************************/
#include "pch.h"
#ifndef RS_OBJECT_BASE_H_
#define RS_OBJECT_BASE_H_

/**
 * @brief Object namespace
 */
namespace RS_Object
{
	/**
	 * @brief Object Base class.
	 */
	class RSObjectBase
	{
	public:
		RSObjectBase() = default;
		virtual ~RSObjectBase() = default;

	public:
		/**
		 * @brief Set name of object.
		 * @param name_ object name (string)
		 */
		void SetName(const std::string& name_) { m_name = name_; }
		/**
		 * @brief Get name of object.
		 * @return object name (string)
		 */
		RS_N_D std::string GetName() const { return m_name; }

	protected:
    std::string m_name; ///< Object name
	};

}

#endif // !RS_OBJECT_BASE_H_