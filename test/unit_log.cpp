////////////////////////////////////////////////////////////////////////////////
// unit_log.cpp
// CLPIM  Aug/6/2008
//
// CL-PIM
// http://code.google.com/p/clpim/ 
//
// Unit test for Log function(log.h and log.cpp).
//
// Revision History:
//
////////////////////////////////////////////////////////////////////////////////


#include "Log.h"

using CLPIM::Log ;
//using CLPIM::logger ;

void foo() ;

extern Log logger ;

int main( void )
{
  cout << "Test Log" << endl ;
  logger.TurnOn() ;
  logger.WriteLog("In Main File") ;
  logger.TurnOff() ;
  logger.WriteLog("Oooops, it has been turned off") ;
  foo() ;
  return 0 ;
}
