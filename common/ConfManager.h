////////////////////////////////////////////////////////////////////////////////
// ConfManager.h
// billjeff  Aug/4/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// This file declare class ConfManager, which manages the configuration files.
// Configuration file manages plugins for CL-PIM. The class provides interfaces
// to remove some plugin from configuration file, or add new plugin.
//
// Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _CONFMANAGER_H_
#define _CONFMANAGER_H_

#include <vector>
#include <string>

#include "GlobalDef.h"

using std::vector ;
using std::string ; 

namespace CLPIM
{
    class ConfManager
    {
    private:
        FILE * conf ;
        vector<string> content ;
    public:
        ConfManager();
        void Run() ;
        void GetPlugInfo(std::vector<std::string> &info);
    } ;
} // namespace CLPIM

#endif // _CONFMANAGER_H_
