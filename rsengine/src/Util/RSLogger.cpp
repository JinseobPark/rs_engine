#include "pch.h"
#include "RSLogger.h"
/*

//
// USE1 with function log
//
    RSLogger time_loger(LOG_LEVEL_INFO);
    time_loger.Ninfo("HI THERE");

//
// USE2 with function log
//
    logger_.Ninfo("HI THERE");


//
// USE1 without function log
//
    RSLogger time_loger(LOG_LEVEL_INFO);
    time_loger.Nmessenger("HI THERE");

//
// USE2 without function log
//
    logger_.Nmessenger("HI THERE");
*/
namespace RS_Logger
{
    RSLogger* mLOGGER = nullptr;

    RSLogger::RSLogger()
    {
      assert(mLOGGER == nullptr && "Only One Logger");
      mLOGGER = this;
      this->log_level_ = LOG_LEVEL_DEBUG;
    }
    RSLogger::RSLogger(const int level)
    {
        this->log_level_ = level;
    }
    void RSLogger::SetLogLevel(const int level)
	  {
		  this->log_level_ = level;
    }
    void RSLogger::WriteMessageLog(const int lv, const char* str, ...) const
    {
      // if release mode, return
#ifdef IS_LOGGER_ON_RELEASE
        FILE* fp = nullptr;
        fp = fopen(__LOG_FILE__, "a");
        if (fp == nullptr)
        {
            puts("fail to open file pointer");
            return;
        }
        char* result = nullptr;
        char level[10];
        switch (lv)
        {
        case(LOG_LEVEL_FATAL): strcpy(level, "[FATAL]"); break;
        case(LOG_LEVEL_WARN): strcpy(level, "[WARN] "); break;
        case(LOG_LEVEL_INFO): strcpy(level, "[INFO] "); break;
        case(LOG_LEVEL_DEBUG): strcpy(level, "[DEBUG]"); break;
        default: 
            break;
        }

        result = static_cast<char*>(malloc(sizeof(char) * (30 + strlen(str) + 18)));
        sprintf(result, "%s %s : %s\n", level, GetTimeStamp().c_str(), str);
        va_list args;
        va_start(args, str);
        vfprintf(fp, result, args);
        va_end(args);
        va_start(args, str);
        if (this->log_level_ >= lv)
        {
            vprintf(result, args);
        }
        va_end(args);

        if (result != nullptr)
        {
            free(result);
        }
        if (fp != nullptr)
        {
            fclose(fp);
        }
        return;
    }

    std::string RSLogger::GetTimeStamp()
    {
        std::string result;
        time_t current_sec = time(nullptr);
        tm* t = localtime(&current_sec);
        std::ostringstream oss;
        switch (t->tm_mon)
        {
        case(0): result = "Jan"; break;
        case(1): result = "Feb"; break;
        case(2): result = "Mar"; break;
        case(3): result = "Apr"; break;
        case(4): result = "May"; break;
        case(5): result = "Jun"; break;
        case(6): result = "Jul"; break;
        case(7): result = "Aug"; break;
        case(8): result = "Sep"; break;
        case(9): result = "Oct"; break;
        case(10): result = "Nov"; break;
        case(11): result = "Dec"; break;
        }
        oss.clear();
        oss << " " << std::setfill('0') << std::setw(2) << t->tm_mday << " " << t->tm_year + 1900;
        oss << " " << std::setfill('0') << std::setw(2) << t->tm_hour;
        oss << ":" << std::setfill('0') << std::setw(2) << t->tm_min;
        oss << ":" << std::setfill('0') << std::setw(2) << t->tm_sec << '\0';
        result = result + oss.str();
        return result;
#endif
    }

    void RSLogger::WriteLog(const char* func_name, int line, const int lv, const char* str, ...) const
    {
#ifdef IS_LOGGER_ON_RELEASE
      FILE* fp = nullptr;
        fp = fopen(__LOG_FILE__, "a");
        if (fp == nullptr)
        {
            puts("fail to open file pointer");
            return;
        }
        char* result = nullptr;
        char level[10];
        switch (lv)
        {
        case(LOG_LEVEL_FATAL): strcpy(level, "[FATAL]"); break;
        case(LOG_LEVEL_WARN): strcpy(level, "[WARN] "); break;
        case(LOG_LEVEL_INFO): strcpy(level, "[INFO] "); break;
        case(LOG_LEVEL_DEBUG): strcpy(level, "[DEBUG]"); break;
		default:
			break;
        }

    #ifdef _DEBUG
        result = static_cast<char*>(malloc(sizeof(char) * (21 + strlen(func_name) + strlen(str) + 30)));
        sprintf(result, "%s %s [%s:%d] : %s\n", level, GetTimeStamp().c_str(), func_name, line, str);
    #else
        result = static_cast<char*>(malloc(sizeof(char) * (30 + strlen(str) + 18)));
        sprintf(result, "%s %s : %s\n", level, GetTimeStamp().c_str(), str);
    #endif
        va_list args;
        va_start(args, str);
        vfprintf(fp, result, args);
        va_end(args);
        va_start(args, str);
        if (this->log_level_ >= lv)
        {
            vprintf(result, args);
        }
        va_end(args);
        if (result != nullptr)
        {
            free(result);
        }
        if (fp != nullptr)
        {
            fclose(fp);
        }
        //return;
#endif
    }

    void RSLogger::PrintLog(const char* func_name, const int line, const int lv, const char* str, ...) const
    {
#ifdef IS_LOGGER_ON_RELEASE
      char* result = nullptr;
        char level[10];
        switch (lv)
        {
        case(LOG_LEVEL_FATAL): strcpy(level, "[FATAL]"); break;
        case(LOG_LEVEL_WARN): strcpy(level, "[WARN] "); break;
        case(LOG_LEVEL_INFO): strcpy(level, "[INFO] "); break;
        case(LOG_LEVEL_DEBUG): strcpy(level, "[DEBUG]"); break;
        default:
          strcpy(level, "[NONE]");
            break;
        }
#ifdef _DEBUG
        result = (char*)malloc(sizeof(char) * (21 + strlen(func_name) + strlen(str) + 30));
        sprintf(result, "%s %s [%s:%d] : %s\n", level, GetTimeStamp().c_str(), func_name, line, str);
#else
        result = static_cast<char*>(malloc(sizeof(char) * (30 + strlen(str) + 18)));
        sprintf(result, "%s %s : %s\n", level, GetTimeStamp().c_str(), str);
#endif
        va_list args;
        va_start(args, str);
        if (this->log_level_ >= lv)
        {
            vprintf(result, args);
        }
        va_end(args);
        if (result != nullptr)
        {
            free(result);
        }
#endif
    }
    bool RSLogger::FatalLog(const char* func_name, int line, int lv, const char* str, ...) const
    {
#ifdef IS_LOGGER_ON_RELEASE
      FILE* fp = nullptr;
      fp = fopen(__LOG_FILE__, "a");
      if (fp == nullptr)
      {
        puts("fail to open file pointer");
        return false;
      }
      char* result = nullptr;
      char level[10];
      switch (lv)
      {
      case(LOG_LEVEL_FATAL): strcpy(level, "[FATAL]"); break;
      case(LOG_LEVEL_WARN): strcpy(level, "[WARN] "); break;
      case(LOG_LEVEL_INFO): strcpy(level, "[INFO] "); break;
      case(LOG_LEVEL_DEBUG): strcpy(level, "[DEBUG]"); break;
      default:
        break;
      }

#ifdef _DEBUG
      result = static_cast<char*>(malloc(sizeof(char) * (21 + strlen(func_name) + strlen(str) + 30)));
      sprintf(result, "%s %s [%s:%d] : %s\n", level, GetTimeStamp().c_str(), func_name, line, str);
#else
      result = static_cast<char*>(malloc(sizeof(char) * (30 + strlen(str) + 18)));
      sprintf(result, "%s %s : %s\n", level, GetTimeStamp().c_str(), str);
#endif
      va_list args;
      va_start(args, str);
      vfprintf(fp, result, args);
      va_end(args);
      va_start(args, str);
      if (this->log_level_ >= lv)
      {
        vprintf(result, args);
      }
      va_end(args);
      if (result != nullptr)
      {
        free(result);
      }
      if (fp != nullptr)
      {
        fclose(fp);
      }
      return false;
#endif
      return false;
    }
}

