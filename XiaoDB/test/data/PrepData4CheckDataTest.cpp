#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdio>

#include <stdint.h>

#include "utility.h"

using namespace std;
using namespace CLPIM;

#define BLOCK_SIZE 8192

int main(void)
{
    bool endian = CheckEndian();
    
    ofstream ofs("CheckDataTest.in", ios::out|ios::binary);
    stringstream ss;
    ss << "0.0.1" << "\02\02\02\02";
    
    int left = BLOCK_SIZE-ss.str().length();

    uint64_t keyNum = 123;
    uint64_t maxKeyNum = 100000000UL;
    uint64_t rootOffset = 1;
    string tmpStr = UInt2Binary(keyNum, endian);
    ss << tmpStr;
    tmpStr = UInt2Binary(maxKeyNum, endian);
    ss << tmpStr;
    tmpStr = UInt2Binary(rootOffset, endian);
    ss << tmpStr;

    left -= 24;
    
    for (int i = 0; i < left; ++i)
        ss << "a";

    ofs << ss.str();

    ofs.close();

    return 0;
}
