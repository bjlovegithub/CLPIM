////////////////////////////////////////////////////////////////////////////////
// utility.h
// billjeff  Aug/28/2008
//
// CL-PIM
// http://code.google.com/p/clpim/
//
// This file declares some useful functions and classes for File management etc. 
//
// Revision History:
//   Aug/28/2008 - Add File management declaration.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "Log.h"
#include "CommonInclude.h"

#include <string>

namespace CLPIM
{
    // Handle file open operation
    FILE * OpenFile( const char *name, const char *mode = "r" ) ;
    // Handl file close operation
    bool CloseFile( FILE *file ) ;
    // Max number of commands
    const int MAX_COMMANDS = 100 ;

    /**
     * Reverse a string
     */
    void ReverseStr(std::string &str);

    /**
     * Check system endian status
     * @return 1 for big endian, 0 for little endian, -1 for unkown endian.
     */
    int8_t CheckEndian();

    /**
     * Convert uint(8, 32, 64)_t into binary form string in little endian.
     * @param i To be converted int.
     * @param endian System endian info.
     * @return Binary form of input int.
     */
    template <class T>
    std::string UInt2Binary(T i, int8_t endian)
    {
        string re(sizeof(i), char(0));
        int8_t c = 0;
        const uint32_t Div = 0x0100;
        while (i) {
            re[c] = char(i % Div);
            i /= Div;
            ++c;
        }
        if (1 == endian) /// big endian
            ReverseStr(re);
        return re;
    }

    /**
     * Convert binary form of uint string back to uint64_t.
     * @param str Binary form of uint string.
     * @return uint64_t value.
     */
    uint64_t Binary2Uint64(const std::string &str);
}

#endif
