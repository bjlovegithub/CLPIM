/**
 * This file is used to quickly write unittest code.
 * Usage:
 *   Define UT function(UT_Case), it must return bool and with no parameter.
 *   Use ADD_CASE macro to add unittest case.
 *   At last, use REGISTER_UT macro to register this UT.
 */

#ifndef UT_TOOL_H
#define UT_TOOL_H

#include <map>
#include <vector>
#include <iostream>

#include "GlobalDef.h"
#include "CommonInclude.h"

typedef bool (*UT_Case)(void);

class UT_Helper
{
public:
    static void AddCase(const std::string &name, UT_Case func)
    {
        sCases[name] = func;
    }

    static bool Run()
    {
        std::vector<std::string> failedCaseName;
        std::map<std::string, UT_Case>::iterator iter;
        for (iter = sCases.begin(); iter != sCases.end(); ++iter)
            if (!iter->second())
            {
                std::cout << "[Case " << iter->first << "]: FAIL\n\n";
                failedCaseName.push_back(iter->first);
            }
            else
                std::cout << "[Case " << iter->first << "]: PASS\n\n";
        std::cout << "### Summary ###\n";
        if (failedCaseName.size() != 0)
        {
            std::cout << failedCaseName.size() << " of " << sCases.size()
                      << " cases FAILED! They are: \n";
            for (size_t i = 0; i < failedCaseName.size(); ++i)
                std::cout << failedCaseName[i] << "\n";
            return false;
        }
        else
            std::cout << "All Cases Passed!" << std::endl;
        return true;
    }

    template <class T>
    static std::string GetStr4UCharStr(const T &arr, size_t len);

private:
    static std::map<std::string, UT_Case> sCases;
};

template <class T>
std::string UT_Helper::GetStr4UCharStr(const T &arr, size_t len)
{
    std::string s = "";
    for (size_t i = 0; i < len; ++i)
        s += (char)(arr[i]);
    return s;
}

template <>
std::string UT_Helper::GetStr4UCharStr(const std::string &arr, size_t len)
{
    std::string s = "";
    for (size_t i = 0; i < len; ++i)
        s += (char)(arr[i]);
    return s;
}


std::map<std::string, UT_Case> UT_Helper::sCases;

#define ADD_CASE(name)                          \
    struct TestCase##name                       \
    {                                           \
        TestCase##name()                        \
        {                                       \
            UT_Helper::AddCase(#name, name);    \
        }                                       \
    } gTestCase##name;

#define TEST_EQUAL(from, to)                                            \
    do {                                                                \
        bool bool_ret_CLPIM_UT = ((from) == (to));                      \
        if (!bool_ret_CLPIM_UT)                                         \
        {                                                               \
            std::cout << "[TEST_EQUAL FAILED @ Line " << __LINE__       \
                      << "]\n" << "Expect: " << (to)                    \
                      << ", Get: " << (from) << std::endl;              \
            return false;                                               \
        }                                                               \
    } while(0);

template <class T>
bool TestArrayEquality(const T &from, const T &to, uint32 len)
{
    for (size_t i = 0; i < len; ++i) {
        if (from[i] != to[i])
            return false;
    }

    return true;
}

template <>
bool TestArrayEquality(const std::string &from, const std::string &to,
                       uint32 len)
{
    for (size_t i = 0; i < len; ++i)
        if (from[i] != to[i])
            return false;
    return true;
}

/// TODO - Add more cases


#define TEST_EQUAL_ARR(from, to, len)                                   \
    do {                                                                \
        bool ret = TestArrayEquality(from, to, len);                    \
        if (!ret) {                                                     \
            std::cout << "[TEST_EQUAL_ARR FAILED @ Line " << __LINE__   \
                      << "]\n" << "Expect: "                            \
                      << UT_Helper::GetStr4UCharStr((to), len)          \
                      << ", Get: "                                      \
                      << UT_Helper::GetStr4UCharStr((from), len)        \
                      << std::endl;                                     \
        }                                                               \
        if (!ret)                                                       \
            return ret;                                                 \
    } while(0);                                 

#define TEST_TRUE(val)                                                  \
    do {                                                                \
        if (!(val)){                                                    \
            std::cout << "[TEST_TRUE FAILED @ Line "                    \
                      << __LINE__ << "]\n";                             \
            return false;                                               \
        }                                                               \
    } while(0);

#define REGISTER_UT(name)                                       \
    int main(void){                                             \
        InitEnv();                                              \
        InitLog();                                              \
        std::string caseName(#name);                            \
        std::cout << "###### " << caseName << " ######\n";      \
        bool re = UT_Helper::Run();                             \
        std::cout << std::endl;                                 \
        if (re)                                                 \
            return 0;                                           \
        return 0;                                               \
    }

#endif // UT_TOOL_H
