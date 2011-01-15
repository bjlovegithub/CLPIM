#include "GlobalDef.h"
#include "FileUtil.h"

#include <pwd.h>
#include <locale.h>

#include <string>
#include <iostream>
#include <cstring>

using namespace std;
using namespace CLPIM;

string HomeDir;
string ConfPath;
string PluginPath;
string UserLocale;

bool InitEnv()
{
    /// Get current user name
    struct passwd *pwd; 
    pwd = getpwuid(getuid());
    if (0 == strcmp("root", pwd->pw_name))
        HomeDir = "/root/";
    else
    {
        HomeDir = "/home/";
        HomeDir += pwd->pw_name;
    }

    ConfPath = HomeDir+"/.CLPIM";

    /// Setup CLPIM path.
    if (!FileUtil::CheckFile(ConfPath))
    {
        if (!FileUtil::CreateFolder(ConfPath))
        {
            cout << "Error when creating conf foler at: " << ConfPath;
            return false;
        }
    }

    PluginPath = ConfPath+"/plugin";
    if (!FileUtil::CheckFile(PluginPath))
    {
        if (!FileUtil::CreateFolder(PluginPath))
        {
            cout << "Error when creating conf foler at: " << PluginPath;
            return false;
        }
    }

    /// Use user's locale.
    const char *userLocale = setlocale(LC_ALL, NULL);
    UserLocale = userLocale;

    return true;
}

bool InitLog()
{
    /// Get current user name
    struct passwd *pwd; 
    pwd = getpwuid(getuid());
    if (0 == strcmp("root", pwd->pw_name))
        HomeDir = "/root/";
    else
    {
        HomeDir = "/home/";
        HomeDir += pwd->pw_name;
    }

    ConfPath = HomeDir+"/.CLPIM";

    /// Setup CLPIM path.
    if (!FileUtil::CheckFile(ConfPath))
    {
        if (!FileUtil::CreateFolder(ConfPath))
        {
            cout << "Error when creating conf foler at: " << ConfPath;
            return false;
        }
    }

    return true;
}
