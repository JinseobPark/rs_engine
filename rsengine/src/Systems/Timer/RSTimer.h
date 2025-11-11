/******************************************************************************/
/*!
\file   RSTimer.h
\author Jinseob Park
\date   2024/08/01

This file contains the declaration of the class RSTimer
This system is for measuring the time of the logic of the game.

*/
/******************************************************************************/
#ifndef RSTIMER_H
#define RSTIMER_H

#include "Systems/RSSystemComponent.h"


/**
 * @brief Timer namespace
 */
namespace RS_Timer
{

	/**
	 * @brief Timer class
	 * This class derived from RSSystemComponent
	 */
	class RSTimer final : public RSSystemComponent
	{
	public:

		/**
		 * @brief Constructor.
		 */
		RSTimer();

		/**
		 * @brief Destructor.
		 */
		~RSTimer() override;


		/**
		 * @brief Initialize the timer.
		 * Set last frame time.
		 */
		void Initialize() override;


		/**
		 * @brief Update the timer.
		 * Calculate delta time and total time.
		 */
		void Update(float) override;


		/**
		 * @brief Shutdown the timer.
		 */
		void Shutdown() override;


		/**
		 * @brief Get delta time.
		 * @return Delta time.
		 */
		RS_N_D float GetDeltaTime() const { return delta_time; }


		/**
		 * @brief Get total time.
		 * @return Total time.
		 */
		RS_N_D float GetTotalTime() const { return total_time; }

	private:
    float delta_time = 0.0f; ///< Delta time.
    float last_frame = 0.0f; ///< Last frame time.
    float total_time = 0.0f; ///< Total time.
	};
  extern RSTimer* mRSTIMER; ///< Timer pointer.
}

#endif // !RSTIMER_H#endif // !RSTIMER_H