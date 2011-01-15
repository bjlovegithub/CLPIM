////////////////////////////////////////////////////////////////////////////////
// unit_utility.cpp
// CLPIM  Sep/6/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// Unit test for utility.cpp. 
//
// Revision History:
//   Sep/6/2008 - Implement the test code.
//
////////////////////////////////////////////////////////////////////////////////

#include "utility.h"
#include "Log.h"

using CLPIM::OpenFile ;
using CLPIM::CloseFile ;
using CLPIM::Log ;

extern Log logger ;

int main( void )
{
  // Test file is not existed
  FILE * f = OpenFile( "in.txt" ) ;
  CloseFile(f) ;
  // Test file is exited
  f = OpenFile( "in.txt" ) ;
  CloseFile(f) ;
  // Test file name is longger than 256.
  /*
  char name[512] ;
  memset( name, 1, 256 ) ;
  name[256] = '\0' ;
  f = OpenFile( name ) ;
  CloseFile(f) ;
  */
  // Test file name is NULL ;
  f = OpenFile( "" ) ;
  CloseFile(f) ;

  return 0 ;
}
