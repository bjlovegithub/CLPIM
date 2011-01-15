#include "utility.h"
#include "ut_tools/ut_tool.h"

#include <string>

using namespace std;
using namespace CLPIM;

bool ReverseStrTest(void)
{
    string str = "";
    ReverseStr(str);
    TEST_EQUAL(str, "");

    str = "ab";
    ReverseStr(str);
    TEST_EQUAL(str, "ba");

    str = "billjeff";
    ReverseStr(str);
    TEST_EQUAL(str, "ffejllib");

    return true;
}
ADD_CASE(ReverseStrTest);

bool CheckEndianTest(void)
{
    int8_t re = CheckEndian();
    cout << (re == 1 ? "BigEndian" : (re == 0 ? "Little" : "Unkown")) << endl;
    return true;
}
ADD_CASE(CheckEndianTest);

bool Binary2Uint64Test(void)
{
    int8_t endian = CheckEndian();
    if (-1 == endian) {
        cout << "Unkown Endian" << endl;
        return true;
    }
    uint8_t ui8 = 100;
    uint64_t re;
    string str = UInt2Binary(ui8, endian);
    const uchar *destArr = (const uchar*)"\144";
    TEST_EQUAL_ARR((const uchar*)str.c_str(), destArr, 1)
    cout << str << endl;
    re = Binary2Uint64(str);
    TEST_EQUAL(re, 100);

    uint32_t ui32 = 1000000;
    str = UInt2Binary(ui32, endian);
    cout << str << endl;
    TEST_EQUAL_ARR(str, string("\100\102\017\0"), 4);
    re = Binary2Uint64(str);
    TEST_EQUAL(re, 1000000);

    uint64_t ui64 = 10000000;
    str = UInt2Binary(ui64, endian);
    re = Binary2Uint64(str);
    TEST_EQUAL(re, 10000000);
    
    return true;
}
ADD_CASE(Binary2Uint64Test);


REGISTER_UT(UtilityTest);

