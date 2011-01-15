////////////////////////////////////////////////////////////////////////////////
// Task.h
// CLPIM  Aug/4/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// This is the main control file for CL-PIM. It manage the configuration file by
// ConfManager object, load the necessray plugins(dynamic lib), and handle error
// conditions etc.
//
// Revision History:
// 
////////////////////////////////////////////////////////////////////////////////

#include "Task.h"
#include "ConfManager.h"
#include "utility.h"
#include "Log.h"
#include "GlobalDef.h"
#include "FileUtil.h"

#include <string>
#include <map>

#include <stdlib.h>
#include <stdio.h>

#include "dlfcn.h"

using namespace std;
using namespace CLPIM;

typedef Task* (*FUNCPTR)() ;

#define GET_POINTER(ptr, name)                  \
    do {                                        \
        ptr = new name;                         \
    } while(0)                                  \

int main(void)
{
    if (!InitEnv())
        exit(1);

    if (!InitLog())
        exit(1);

    /// Map for plugins and their Makers and Cleaners.
    map<string, Task*> plugins;
    map<string, FUNCPTR> makers;
    map<string, FUNCPTR> cleaners;
    map<string, void*> handlers;
    ConfManager cm;
    vector<string> plugInfo;
    cm.GetPlugInfo(plugInfo);
    /// Load plugins
    for (size_t i = 0; i < plugInfo.size(); ++i)
    {
        string libName("lib");
        libName += plugInfo[i] + ".so";
        string libPath = PluginPath + "/" + libName;
        void *dp = dlopen(libPath.c_str(), RTLD_LAZY);
        if (dp == 0){
            //logger.WriteLog(dlerror()) ;
            LOG_DEBUG(dlerror());
            exit(1);
        }
        handlers[plugInfo[i]] = dp;

        // get the Maker function.
        FUNCPTR maker_ptr = 0;
        maker_ptr = (FUNCPTR)dlsym(dp, "Maker");
        const char *error_info = dlerror();
        if (error_info){
            //logger.WriteLog(error_info) ;
            LOG_DEBUG(error_info);
            exit(1);
        }

        FUNCPTR cleaner_ptr = 0;
        cleaner_ptr = (FUNCPTR)dlsym(dp, "Cleaner");
        error_info = dlerror();
        if (error_info){
            //logger.WriteLog(error_info) ;
            LOG_DEBUG(error_info);
            exit(1) ;
        }

        // put it into map
        plugins[plugInfo[i]] = maker_ptr();
        if (NULL == plugins[plugInfo[i]])
            LOG_ERROR("Load plugin error: " << plugInfo[i]);
        makers[plugInfo[i]] = maker_ptr;
        cleaners[plugInfo[i]] = cleaner_ptr;
    }

    /// users loop.
    map<string, Task*>::iterator it ;
    while (1)
    {
        string commands[MAX_COMMANDS];
        cout << "Available functions are: \n" ;
        {
            int c ;
            for (c = 0, it = plugins.begin();
                 it != plugins.end();
                 ++it, ++c)
            {
                cout << c << ": " << (*it).first << endl;
                commands[c] = it->first;
            }
            cout << c << ": Configuration" << endl;
            cout << c+1 << ": Quit" << endl;
        }
        cout << "Please enter your selection[0, 1, 2, 3...]: " ;
        size_t c ;
        {
            string tmp;
            cin >> tmp;
            if (sscanf(tmp.c_str(), "%u", &c) < 1)
            {
                cout << "Invalid Command: " << tmp << endl;
                continue;
            }
            /// check user input is in the valid range
            if (c >= plugins.size()+2)
            {
                cout << "Invalid Command, out of range: " << tmp << endl;
                continue;
            }
        }

        if (c < plugins.size())
            plugins[commands[c]]->Exec();
        else if (c == plugins.size()+1)
        {
            cout << "Quit." << endl;
            break;
        }
        else
        {
            cm.Run();
        }
    }

    /// clean up
    {
        typeof(plugins.begin()) iter;
        for (iter = plugins.begin(); iter != plugins.end(); ++iter)
        {
            const string &name = iter->first;
            cleaners[name]();
            dlclose(handlers[name]);
        }
    }

    return 0 ;
}
