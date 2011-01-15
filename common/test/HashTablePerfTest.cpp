/**
 * Test the performance of HastTable.
 * by billjeff, Dec/1/2010
 */

#include "HashTable.hpp"
#include "Time.h"

#include <iostream>

using namespace CLPIM;
using namespace std;

int main(void)
{
    SimpleHashTable<int32_t, string> ht;
    /// insert 10000000 records and get the time consuming
    int32_t recNum = 3000000;
    uint64_t start = Timer::GetCurrentTimeInMS();
    uint64_t end;
    for (int32_t i = 0; i < recNum; ++i)
        ht.Insert(i, "abc");
    end = Timer::GetCurrentTimeInMS();
    cout << "start: " << start << ", end: " << end << endl;
    cout << "Time: " << end-start << endl;

    cout << "Check key is in hash table or not, get the check key time" << endl;
    start = Timer::GetCurrentTimeInMS();
    string value;
    bool allKeyInTable, allValueIsOk;
    allKeyInTable = allValueIsOk = true;
    for (int32_t i = 0; i < recNum; ++i) {
        if (!ht.CheckKey(i, value)) {
            allKeyInTable = false;
            continue;
        }
        if (value != "abc")
            allValueIsOk = false;
    }
    end = Timer::GetCurrentTimeInMS();
    cout << "Time: " << end-start << endl;
    cout << "All key is in table(expect true): " << allKeyInTable << endl;
    cout << "All value is Ok(expect true): " << allValueIsOk << endl;

    bool f = ht.CheckKey(-1, value);
    cout << "Not in table(expect false): " << f << endl;

    cin >> f;
    
    return 0;
}
