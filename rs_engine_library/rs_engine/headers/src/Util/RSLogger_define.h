#ifndef RS_LOGGER_DEFINE_H_
#define RS_LOGGER_DEFINE_H_

#define LOG_LEVEL_OFF 0
#define LOG_LEVEL_FATAL 10
#define LOG_LEVEL_WARN 20
#define LOG_LEVEL_DEBUG 30
#define LOG_LEVEL_INFO 40
#define LOG_LEVEL_ALL 100
#define __LOG_FILE__ "../RSEngine_report.log"

#define RSfatal(str, ...) FatalLog(__FUNCTION__, __LINE__, LOG_LEVEL_FATAL, str, ##__VA_ARGS__)
#define RSwarn(str, ...) WriteLog(__FUNCTION__, __LINE__, LOG_LEVEL_WARN, str, ##__VA_ARGS__)
#define RSdebug(str, ...) PrintLog(__FUNCTION__, __LINE__, LOG_LEVEL_DEBUG, str, ##__VA_ARGS__)
#define RSprint(str, ...) PrintLog(__FUNCTION__, __LINE__, LOG_LEVEL_OFF, str, ##__VA_ARGS__)
#define RSinfo(str, ...) WriteLog(__FUNCTION__  , __LINE__, LOG_LEVEL_INFO, str, ##__VA_ARGS__)

#define RSmessenger(str, ...) WriteMessageLog(LOG_LEVEL_INFO, str, ##__VA_ARGS__)


#define RS_FATAL RS_Logger::mLOGGER->RSfatal
#define RS_WARN RS_Logger::mLOGGER->RSwarn
#define RS_INFO RS_Logger::mLOGGER->RSinfo
#define RS_DEBUG RS_Logger::mLOGGER->RSdebug

#define RS_MSG RS_Logger::mLOGGER->RSmessenger

#define RS_PRINT RS_Logger::mLOGGER->RSprint

#ifdef NDEBUG
#define RS_ASSERT(condition, message) ((void)0)
#else
#define RS_ASSERT(condition, message) assert((condition) ? true : RS_FATAL(message))
#endif

#endif // RS_LOGGER_DEFINE_H_
