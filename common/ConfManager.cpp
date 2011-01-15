////////////////////////////////////////////////////////////////////////////////
// ConfManager.cpp
// CLPIM  Aug/14/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// Implementation of class ConfManager.
//
// Revision History:
// 
////////////////////////////////////////////////////////////////////////////////

#include "ConfManager.h"
#include "Log.h"

#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace CLPIM;
using namespace std;

ConfManager::ConfManager()
{
    // open CONF file.
    string filePath = ConfPath+"/conf";
    conf = fopen(filePath.c_str(), "r") ;
    if ( !conf ) {
        //logger.WriteLog( "Open conf.ini error! Create new one instead." ) ;
        LOG_DEBUG("Open conf.ini error! Create new one instead.");
        conf = fopen( filePath.c_str(), "w" ) ;
        if ( !conf ){
            //logger.WriteLog( "Unable to create conf.ini, CL-PIM exits abnormal!" ) ;
            LOG_DEBUG("Unable to create conf.ini, CL-PIM exits abnormal!");
            exit(1) ;
        }
    }

    // read CONF content
    {      
        char buf[1024] ;
        while ( fscanf( conf, "%s\n", buf ) != EOF ){
            if (strlen(buf) > 1 && buf[0] != '#'){
                string s(buf) ;
                content.push_back(s) ;
            }
        }
        fclose( conf ) ;
    }
}

// main controller
void ConfManager::Run()
{
    string c ;
    // if changed is true, write the configuration to conf file.
    bool changed = false ;
    while (1){
        cout << "======\n" ;
        cout << "Available Commands for Configuration Management:\n" ;
        cout << "[1] Add Plugin\n" ;
        cout << "[2] Remove Plugin\n" ;
        cout << "[3] Display All Plugins\n" ;
        cout << "[4] Exit Configuration Management\n" ;
        cout << "Please Enter the Commands[1,2,3,4]: " ;
        cin >> c ;
        if (c.size() < 1 || c.size() > 1 || 
            (c[0] != '4' && c[0] != '1' && c[0] != '2' && c[0] != '3'))
        {
            cout << "Invalid Command" << endl;
            continue ;
        } 

        char comm = c[0] ;
        if ( comm == '1' ){
            cout << "Put the plugin in the plugin directory, and enter the plugin"
                 << "\nname without prefix lib and .so postfix." << endl ;
            string plugin_name ;
            cout << "Enter the plugin name(case sensitive): " ;
            cin >> plugin_name ;
            // solve the duplicate condition
            if (find(content.begin(), content.end(), plugin_name) != content.end())
            {
                //logger.WriteLog( plugin_name+" is already installed!" ) ;
                LOG_DEBUG(plugin_name + " is already installed!");
                continue ;
            }

            // for the full directory for this plugin.
            string fullName = "lib" + plugin_name + ".so";
            string plugin_dir = ConfPath+"/plugin/"+fullName;
            FILE * tmp_file = fopen(plugin_dir.c_str(), "r");
            if (!tmp_file){
                LOG_DEBUG("Plugin file name error: " + plugin_name +
                          ", please" +" check the file name" ) ;
                continue ;
            }
            LOG_DEBUG("Plugin - " + plugin_name + " has been installed successfully!");
            LOG_DEBUG("Restart the CL-PIM to use this plugin!");                          
            content.push_back(plugin_name) ;
            changed = true ;
        }
        else if ( comm == '2' ){
            string name ;
            cout << "Enter the plugin name which you want to remove(case sensitive): " 
                 << endl ;
            cin >> name ;
            vector<string>::iterator it = find( content.begin(), content.end(), name ) ;
            if ( it == content.end() ){
                //logger.WriteLog( "No such plugin existed: "+name ) ;
                LOG_DEBUG("No such plugin existed: " + name);
                continue ;
            }
            content.erase(it);
            //logger.WriteLog( "Plugin: "+name+ " has been removed successfully!" ) ;
            LOG_DEBUG("Plugin: " + name +
                      " has been removed successfully!");
            changed = true ;
        }
        else if ( comm == '3' ){
            cout << "Installed Plugins Are: " << endl ;
            int size = content.size() ;
            for ( int i = 0 ; i < size ; ++i ){
                cout << i+1 << ": " << content[i] << endl ;
            }
            //logger.WriteLog( "Plugins list successfully!" ) ;
            LOG_DEBUG("Plugins list successfully!");
        }
        else if ( comm == '4' ){
            if ( changed ){
                // write plugin info to configuration file
                // open CONF file.
                string filePath = ConfPath + "/conf";
                conf = fopen( filePath.c_str(), "w" ) ;
                if ( !conf ){
                    //logger.WriteLog( "Unable to create conf.ini, CL-PIM exits abnormal!" ) ;
                    LOG_DEBUG("Unable to create conf file, CL-PIM exits abnormal!");
                    exit(1) ;	    
                }
                int size = content.size() ;
                for ( int i = 0 ; i < size ; ++i )  
                    fprintf( conf, "%s\n", content[i].c_str() ) ;
                fclose( conf ) ;
                LOG_DEBUG("Write the configuration info successfully to "
                          + filePath);
            }
            break ;
        }
    }
}

void ConfManager::GetPlugInfo(vector<string> &info)
{
    info = content;
}
