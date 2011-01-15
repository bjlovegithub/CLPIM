#ifndef CLPIM_GLOBAL_DEF_H
#define CLPIM_GLOBAL_DEF_H

#include <string>

extern std::string HomeDir;
extern std::string ConfPath;
extern std::string PluginPath;
extern std::string ConfDir;

/**
 * Initialize enviroment, including log path, folder preparation.
 */
bool InitEnv();

/**
 * Initialize log configuration.
 */
bool InitLog();

#endif // CLPIM_GLOBAL_DEF_H
