////////////////////////////////////////////////////////////////////////////////
// TODOManager.cpp
// CLPIM  Sep/6/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// This file defines the functions which are declared in utility.h.
//
// Revision History:
//   
//
////////////////////////////////////////////////////////////////////////////////

#include "TODOManager.h"
#include <iostream>
using namespace std ;

namespace CLPIM
{
    // TODO - Improve it in the future.
    void TODOManager::Exec()
    {
        cout << "Hello, it works!" << endl ;
    }

    // TODO - Use smart pointer is a better choice.
    Task * Maker()
    {
        return new TODOManager ;
    }
}
