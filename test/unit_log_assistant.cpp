////////////////////////////////////////////////////////////////////////////////
// unit_log_assitant.cpp
// CLPIM  Aug/6/2008
//
// CL-PIM
// http://code.google.com/p/clpim/ 
//
// Assistant file for unit_log.cpp
//
// Revision History:
//
////////////////////////////////////////////////////////////////////////////////



#include "Log.h"

using CLPIM::Log ;
//using CLPIM::logger ;

extern Log logger ;

void foo()
{
  logger.TurnOn() ;
  cout << "In foo" << endl ;
  logger.WriteLog("In foo") ;
}
