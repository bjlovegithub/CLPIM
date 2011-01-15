////////////////////////////////////////////////////////////////////////////////
// TODOManager.h
// CLPIM  Sep/6/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// TODOManager Plugin.
//
// Revision History:
//   Sep/6/2008 - Declare todo_file and Exec.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _TODOMANAGER_H_
#define _TODOMANAGER_H_


#include "Task.h"


namespace CLPIM
{
    class TODOManager : public Task
    {
    private:
        // File which is used to store TODOs.
        FILE * todo_file ;
    public:
        // main control fucntion
        void Exec() ;
    } ;

    // make a pointer of TODOManager.
    // Task * Maker() ;
}

#endif // _TODOMANAGER_H_
