////////////////////////////////////////////////////////////////////////////////
// Log.h
// CLPIM  Aug/4/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// This file includes the declaration of class Log. class Log is used to make
// logs during the program execution.
//
// Revision History:
//   Aug/26/2008 - Add the implementation of WriteLog(string).
////////////////////////////////////////////////////////////////////////////////


#ifndef _LOG_H_
#define _LOG_H_

#include "calendar.h"
#include "GlobalDef.h"

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ENABLE_LOG

namespace CLPIM
{
    /// const char LOGFILE[] = CLPIM_CONF_PATH"log" ;
    class Log
    {
    public:
        enum LEVEL {DEBUG, INFO, ERROR, FATAL};
    private:
        int f_;
        // if verbose_ is true, then WriteLog can take effect, otherwise,
        // no log info will be generated.
        bool verbose_ ;
        Calendar c_ ;
        LEVEL mLevel;
    public:
        Log() ;
        ~Log() ;
        void TurnOn();
        void TurnOff();
        void WriteLog(const char *, LEVEL l);
        void WriteLog(const std::string &s, LEVEL l);
        void SetLevel(LEVEL l);
    private:
        std::string LevelToStr(LEVEL l);
    };

    class LogManager
    {
    public:
        static Log* GetInstance()
        {
            if (!sLogger)
                sLogger = new Log;
            return sLogger;
        }
    private:
        static Log *sLogger;
        LogManager() {}
        LogManager(const LogManager &rhs) {}
    };
} // namespace CLPIM

#ifdef ENABLE_LOG
#define LOG(content, l)                                             \
    do {                                                            \
        stringstream ss;                                            \
        ss << __FILE__ << ":" << __LINE__                           \
           << " [" << __FUNCTION__ << "] " << content;              \
        CLPIM::LogManager::GetInstance()->WriteLog(ss.str(), l); \
    } while(0)

#define LOG_DEBUG(content) LOG(content, CLPIM::Log::DEBUG)
#define LOG_INFO(content) LOG(content, CLPIM::Log::INFO)
#define LOG_ERROR(content) LOG(content, CLPIM::Log::ERROR)
#define LOG_FATAL(content) LOG(content, CLPIM::Log::FATAL)

#define SET_LOG_LEVEL(l) CLPIM::LogManager::GetInstance()->SetLevel(l)
    
#else
#define LOG(content, l)
#define LOG_DEBUG(content)
#define LOG_INFO(content)
#define LOG_ERROR(content)
#define LOG_FATAL(content)
#define SET_LOG_LEVEL(l)
#endif

#ifdef _DEBUG
#define DebugPrint(val)                                                 \
    std::cout << __FILE__ << ": " << __LINE__ << " "                    \
    << __FUNCTION__ << " " << #val << ": " << val << std::endl;
#else
#define DebugPrint(val)
#endif

#endif // _LOG_H_
