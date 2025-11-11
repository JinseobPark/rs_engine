/******************************************************************************/
/*!
\file   RSSystemComponent.h
\author Jinseob Park
\date   2024/08/01

This file contains system component base class for system component

*/
/******************************************************************************/
#ifndef RS_SYSTEM_COMPONENT_H_
#define RS_SYSTEM_COMPONENT_H_

#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <cassert>

/**
 * @brief System Component for RS Engine
 */
class RSSystemComponent
{
public:
	/**
	 * @brief Initialize RSComponent
	 */
	virtual void Initialize() = 0;

	/**
	 * @brief PostInitialize RSComponent
	 */
	virtual void PostInitialize() {}

	/**
	 * @brief Update RSComponent
	 * @param dt delta time
	 */
	virtual void Update(float dt) = 0;

	/**
	 * @brief Shutdown RSComponent
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Destructor for RSComponent
	 */
	virtual ~RSSystemComponent() = default;

}; // class RSSystemComponent

#endif // !RS_SYSTEM_COMPONENT_H_