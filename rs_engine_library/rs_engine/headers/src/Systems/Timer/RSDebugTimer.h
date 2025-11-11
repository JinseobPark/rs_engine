/******************************************************************************/
/*!
\file   RSDebugTimer.h
\author Jinseob Park
\date   2024/08/01

This file contains the declaration of the class RSDebugTimer
This system is for measuring the debugging time or code running time.

*/
/******************************************************************************/
#ifndef RSDEBUGTIMER_H
#define RSDEBUGTIMER_H

#include <time.h>
#include <chrono>
#include <iostream>


/**
 * @brief Timer namespace
 */
namespace RS_Timer
{

	/**
	 * @brief Debug Timer class
	 * This class is for measuring the debugging time or code running time.
	 * It is a singleton class. Anywhere in the code, you can call the function to measure the time.
	 * Timer's precision is milliseconds.
	 */
	class RSDebugTimer
	{
	public:

		/**
		 * @brief GetInstance
		 * @return RSDebugTimer*
		 */
		static RSDebugTimer* GetInstance();


    /**
     * @brief Release the instance
		 */
		static void Release();

		/**
		 * @brief Start the timer
		 */
		void Start();

		/**
		 * @brief End the timer
		 */
		void EndNPrint();


		/**
		 * @brief End the timer and print the message
		 * @param message_ Message to print
		 */
		void EndNPrint(const std::string& message_);


    /**
     * @brief End the time and print the message to log
     * @param message_ Message to log
     */
    void EndNLog(const std::string& message_);

	private:
		RSDebugTimer() = default;
		~RSDebugTimer() = default;
    static RSDebugTimer* m_instance; ///< Instance

		// milliseconds timer
    std::chrono::time_point<std::chrono::system_clock> start_time; ///< Start time
    std::chrono::time_point<std::chrono::system_clock> end_time; ///< End time
    std::chrono::duration<double> elapsed_time; ///< Elapsed time

	};



}

#endif // !RSDEBUGTIMER_H