/******************************************************************************/
/*!
\file   RSLogger.h
\author Jinseob Park
\date   2024/08/01

This file contains logger for logging the message.
This system write the log message to the file.

*/
/******************************************************************************/

#ifndef RSLOGGER_H
#define RSLOGGER_H

#define IS_LOGGER_ON_RELEASE 1

#include <iomanip>
#include <fstream>
#include <cstdarg>
#include <ctime>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <time.h>
#include "RSLogger_define.h"


/**
 * @brief Logger namespace
 */
namespace RS_Logger
{
	/**
	 * @brief Logger class
     */
    class RSLogger {
    public:
	    /**
		 * @brief Constructor for RSLogger
         */
        RSLogger();

	    /**
		 * @brief Constructor for RSLogger
		 * @param level log level
         */
	    explicit RSLogger(int level);


	    /**
		 * @brief Set log level to cmd
		 * @param level log level
		 */
		void SetLogLevel(int level);

	    /**
		 * @brief Write log message
         * @param func_name function name
		 * @param line line number
		 * @param lv log level
		 * @param str log message
		 * @param ... variable arguments
         * @return false for assert.
         */
		void WriteLog(const char* func_name, int line, int lv, const char* str, ...) const;


	    /**
		 * @brief Print Log message to console
		 * @param func_name	function name
		 * @param line line number
		 * @param lv log level
		 * @param str log message
		 * @param ... variable arguments
		 * @return false for assert.
		 */
		void PrintLog(const char* func_name, int line, int lv, const char* str, ...) const;


      /**
     * @brief Debug Log
     * @param func_name function name 
     * @param line line number
     * @param lv log level
     * @param str log message
     * @param ... variable arguments
     * @return false for assert.
     */
    bool FatalLog(const char* func_name, int line, int lv, const char* str, ...) const;

	    /**
		 * @brief Write message log
		 * @param lv log level
		 * @param str log message
		 * @param ... variable arguments
         */
    void WriteMessageLog(int lv, const char* str, ...) const;

    protected:
		int log_level_; /**< log level */
		static std::string GetTimeStamp(); /**< get time stamp */
    };
    extern RSLogger* mLOGGER; /**< logger instance */
}

#endif // !RSLOGGER_H