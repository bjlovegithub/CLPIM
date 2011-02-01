////////////////////////////////////////////////////////////////////////////////
// Log.cpp
// CLPIM  Aug/4/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// This file includes the definition of class Log. 
//
// Revision History:
//   Aug/6/2008 - Add TurnOn and TurnOff function.
//   Aug/26/2008 - Add function WriteLog(string).
// 
////////////////////////////////////////////////////////////////////////////////

#include "Log.h"

#include <string>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;
using namespace CLPIM;

// ctor for class Log.
Log::Log()
{
    verbose_ = true;
    string logPath = ConfPath+"/log";
    f_ = open(logPath.c_str(), O_RDWR|O_CREAT|O_APPEND);
    chmod(logPath.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    if (-1 == f_) {
        cerr << "Open file " << logPath.c_str() << " error!" << endl ;
        exit(1);
    }
    //fcntl(f_, S_IRWXU | S_IRWXG | S_IRWXO);
    mLevel = DEBUG;
}

// dtor
Log::~Log()
{
    close(f_) ;
}

// Trun on the log function.
void Log::TurnOn()
{
    verbose_ = true ;
}

// Turn off log function.
void Log::TurnOff()
{
    verbose_ = false ;
}

// Write information to terminal and log files.
void Log::WriteLog(const char *info, LOG_LEVEL l)
{
    if (verbose_ && info != 0 && l >= mLevel){
        string s = "[LOG " ;
        s += LevelToStr(l) + " ";
        s += c_.TimeWithoutTimeZone(true) + "]: " + info + "\n" ;
        write(f_, s.c_str(), s.size());
    }
}

void Log::WriteLog(const string &s, LOG_LEVEL l)
{
    WriteLog(s.c_str(), l) ;
}

void Log::SetLevel(LOG_LEVEL l)
{
    mLevel = l;
}

string Log::LevelToStr(LOG_LEVEL l)
{
    string str;
    switch(l)
    {
    case DEBUG:
        str = "DEBUG";
        break;
    case INFO:
        str = "INFO ";
        break;
    case ERROR:
        str = "ERROR";
        break;
    case FATAL:
        str = "FATAL";
        break;
    }
    return str;
}

Log::LogCallTmpClass::LogCallTmpClass(const std::string &f, int l,
                                      const std::string &fn, LOG_LEVEL level)
{
    mFileName = f;
    mLineNum = l;
    mFuncName = fn;
    mLevel = level;
    stringstream ss;
    ss << mFileName << ":" << mLineNum
       << " [" << mFuncName << "] " << "Call " << mFuncName;
    CLPIM::LogManager::GetInstance()->WriteLog(ss.str(), mLevel);
}

Log::LogCallTmpClass::~LogCallTmpClass()
{
    stringstream ss;
    ss << mFileName << ":" << mLineNum
       << " [" << mFuncName << "] " << "Leave " << mFuncName;
    CLPIM::LogManager::GetInstance()->WriteLog(ss.str(), mLevel);
}


Log* LogManager::sLogger = NULL;
