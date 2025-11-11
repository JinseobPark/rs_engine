/******************************************************************************/
/*!
\file   RSComponentType.h
\author Jinseob Park
\date   2024/08/01

This file contains component type enum class for object.

*/
/******************************************************************************/
#ifndef RS_COMPONENT_TYPE_H_
#define RS_COMPONENT_TYPE_H_

/**
 * @brief Component type enum class
 */
enum class RSComponentType : std::uint8_t
{
	CT_NONE = 0,
	CT_MODEL,
	CT_MOVE,
	CT_PARTICLE,
	CT_RIGIDBODY,
	CT_COUNT
};

#endif // !RS_COMPONENT_TYPE_H_