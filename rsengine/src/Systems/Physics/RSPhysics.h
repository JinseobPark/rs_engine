/******************************************************************************/
/*!
\file   RSPhysics.h
\author Jinseob Park
\date   2024/08/01

This file contains Physics System for Game engine and logic part.

*/
/******************************************************************************/
#ifndef RS_PHYSICS_H_
#define RS_PHYSICS_H_

#include "Systems/RSSystemComponent.h"
#include "Resource/Manager/RSResourceManager.h"



/**
 * @brief Physics namespace
 */
namespace RS_Physics
{

	/**
	 * @brief Physics Engine System
	 */
	class RSPhysics final : public RSSystemComponent
	{
	public:

		/**
		 * @brief Construct
		 */
		RSPhysics();

		/**
		 * @brief Copy Construct
		 */
		RSPhysics(const RSPhysics&) = delete;

		/**
		 * @brief Copy Assignment
		 * @return RSPhysics&
		 */
		RSPhysics& operator=(const RSPhysics&) = delete;

		/**
		 * @brief Destructor
		 */
		~RSPhysics() override;


		/**
		 * @brief Initialize Physics System. Do nothing.
		 */
		void Initialize() override;

		/**
		 * @brief Post Initialize Physics System. Get Resource Manager and Object Manager.
		 */
		void PostInitialize() override;


		/**
		 * @brief Update Physics System. Update Rigidbody Component.
		 * @param dt Delta Time
		 */
		void Update(float) override;

		/**
		 * @brief Shutdown Physics System. Set Resource Manager and Object Manager to nullptr.
		 */
		void Shutdown() override;

	private:
    RSResourceManager* m_resource_manager = nullptr; ///< Resource Manager Pointer
    RS_Object::RSObjectManager* m_object_manager = nullptr; ///< Object Manager Pointer

	};
  extern RSPhysics* mRSPHYSICS; ///< Physics System Pointer

}

#endif // RS_PHYSICS_H_#endif // RS_PHYSICS_H_