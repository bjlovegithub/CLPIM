#include "HashTable.hpp"

#include "ut_tools/ut_tool.h"

#include <iostream>
#include <algorithm>

using namespace std;

namespace CLPIM
{

bool NormalizeSizeTest(void)
{
    SimpleHashTable<> table;
    unsigned int ret;

    ret = table.NormalizeSize(0);
    TEST_EQUAL(ret, 1);

    ret = table.NormalizeSize(1);
    TEST_EQUAL(ret, 1);

    ret = table.NormalizeSize(3);
    TEST_EQUAL(ret, 4);

    ret = table.NormalizeSize(10);
    TEST_EQUAL(ret, 16);

    return true;
}
ADD_CASE(NormalizeSizeTest);

bool GetHashKeyTest(void)
{
    SimpleHashTable<> table;
    table.mTableSize = 3;
    table.GetHashKey();
    TEST_EQUAL(table.mHashKey, 2);

    table.mTableSize = 10;
    table.GetHashKey();
    TEST_EQUAL(table.mHashKey, 7);

    table.mTableSize = 100;
    table.GetHashKey();
    TEST_EQUAL(table.mHashKey, 97);

    table.mTableSize = 30;
    table.GetHashKey();
    TEST_EQUAL(table.mHashKey, 29);

    return true;
}
ADD_CASE(GetHashKeyTest);

bool InsertTest(void)
{
    SimpleHashTable<> h(7);
    h.Insert(1, 1);
    TEST_EQUAL(h.mTableSize, 8);
    TEST_EQUAL(h.mHashKey, 7);
    TEST_EQUAL(h.mItemNum, 1);
    int32_t val;
    bool re = h.CheckKey(1, val);
    TEST_EQUAL(val, 1);
    TEST_EQUAL(re, true);

    h.Insert(1, 11);
    TEST_EQUAL(h.mTableSize, 8);
    TEST_EQUAL(h.mHashKey, 7);
    TEST_EQUAL(h.mItemNum, 1);
    re = h.CheckKey(1, val);
    TEST_EQUAL(val, 11);
    TEST_EQUAL(re, true);

    h.Insert(2, 2);
    h.Insert(3, 2);
    h.Insert(4, 2);
    h.Insert(5, 2);
    h.Insert(6, 2);
    h.Insert(66, 7);
    h.Insert(7, 7);    
    TEST_EQUAL(h.mTableSize, 16);
    TEST_EQUAL(h.mHashKey, 13);
    TEST_EQUAL(h.mItemNum, 8);
    re = h.CheckKey(7, val);
    TEST_EQUAL(val, 7);
    TEST_EQUAL(re, true);
    
    re = h.CheckKey(17, val);
    TEST_EQUAL(re, false);

    string destStr = "123456766";
    vector<int32> ret;
    h.GetAllKeys(ret);
    sort(ret.begin(), ret.end());
    stringstream ss;
    for (size_t i = 0; i < ret.size(); ++i) {
        ss << ret[i];
    }
    TEST_EQUAL(ss.str(), destStr);

    return true;
    
}
ADD_CASE(InsertTest);


bool CheckKeyTest(void)
{
    SimpleHashTable<> h(100);
    h.Insert(10003, 1);
    h.Insert(123123, 21312);

    bool re;
    int value;
    re = h.CheckKey(10003, value);
    TEST_EQUAL(re, true);
    TEST_EQUAL(value, 1);    
    re = h.CheckKey(10000, value);
    TEST_EQUAL(re, false);
    re = h.CheckKey(123123, value);
    TEST_EQUAL(re, true);
    TEST_EQUAL(value, 21312);
    h.RemoveKey(123123);
    re = h.CheckKey(123123, value);
    TEST_EQUAL(re, false);
    
    return true;
}
ADD_CASE(CheckKeyTest);

}/// namespace CLPIM

REGISTER_UT(HashTableTest);

