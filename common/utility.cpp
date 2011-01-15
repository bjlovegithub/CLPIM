////////////////////////////////////////////////////////////////////////////////
// utility.cpp
// billjeff  Aug/28/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// This file defines the functions which are declared in utility.h.
//
// Revision History:
//   Aug/28/2008 - Add the definition of OpenFile and CloseFile.
//
////////////////////////////////////////////////////////////////////////////////


#include "utility.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;

const int MAX_NAME_LENGTH = 256 ;
const char MSG_CREATE_NEW[] = " is not existed, create a new one instead!" ;
const char MSG_ERROR[] = "Fetal Error! Exit CL-PIM. Cannot create file: " ; 

// Implementation fo OpenFile
FILE * CLPIM::OpenFile( const char *name, const char *mode )
{
    int name_len = 0 ;
    // check the legitimation of file name.
    {
        if ( !name ){
            //logger.WriteLog( "File name is invalid! Fetal Error!" ) ;
            LOG_ERROR("File name is invalid! Fatal Error!");
            return NULL;
        }
        // check the length of file name
        bool f = true ;
        for ( int i = 0 ; i < MAX_NAME_LENGTH ; ++i, ++name_len )
            if ( name[i] == '\0' ){	  
                f = false ;
                break ;
            }
        if ( f ){
            //logger.WriteLog( "File name is too long! Fetal Error!" ) ;
            LOG_DEBUG("File name is too long!");
            return NULL;
        }
        // file name is empty...
        if ( name_len == 0 ){
            //logger.WriteLog( "File name is too short! Fetal Error!" ) ;
            LOG_DEBUG("File name is too short!");
            return NULL;
        }
    }
    FILE * f = fopen( name, mode ) ;
    // file is not existed, write information to terminal and log file.
    if ( !f ){
        char buf[MAX_NAME_LENGTH*2] ;      
        strncpy( buf, name, name_len ) ;

        strncpy( buf+name_len, MSG_CREATE_NEW, strlen(MSG_CREATE_NEW) ) ;  
        buf[name_len+strlen(MSG_CREATE_NEW)] = '\0' ;
        //logger.WriteLog(buf) ;
        LOG_DEBUG(buf);
        // create a new file.
        f = fopen( name, "w" ) ;
        // if file cannot be created, exit CL-PIM
        if ( !f ){
            int len = strlen(MSG_ERROR) ;
            strncpy( buf, MSG_ERROR, len ) ;
            strncpy( buf+len, name, name_len ) ;
            buf[len+name_len] = '\0' ;
            //logger.WriteLog(buf) ;
            LOG_DEBUG(buf);
        }      
    }
    return f ;
}

bool CLPIM::CloseFile( FILE *file )
{
    if ( !file )  return false ;
    	
    if ( fclose(file) == 0 ){      
        //logger.WriteLog("Close file successfully in CloseFile Func!");
        LOG_DEBUG("Close file successfully in CloseFile Func!");
        return true ;
    }
    return false ;
}

int8_t CLPIM::CheckEndian()
{
    uint32_t val = 0x01020304;
    uint8_t *ptr = (uint8_t*)&val;
    /// big endian case
    if (*ptr == 1 && *(ptr+1) == 2 && *(ptr+2) == 3 && *(ptr+3) == 4)
        return 1;
    if (*ptr == 4 && *(ptr+1) == 3 && *(ptr+2) == 2 && *(ptr+3) == 1)
        return 0;
    return -1;
}

void CLPIM::ReverseStr(string &str)
{
    if (str.size() < 2)
        return;
    size_t start = 0, end = str.size()-1;
    while (start < end) {
        swap(str[start], str[end]);
        ++start;
        --end;
    }
}

uint64_t CLPIM::Binary2Uint64(const string &str)
{
    uint64_t val = 0;
    uint64_t exp = 1;
    size_t len = str.size();
    for (int i = 0; i < len; ++i) {
        val += exp * uint8_t(str[i]);
        exp <<= 8;
    }
    return val;
}
