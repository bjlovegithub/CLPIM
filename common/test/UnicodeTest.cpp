//-*- mode: gbk -*-

#include "Unicode.h"
#include "Log.h"
#include "ut_tools/ut_tool.h"

#include <string.h>

using namespace CLPIM;

bool ConverterTest(void)
{
    char src[] = "中文测试";
    char *destPtr = CharSetConvert("utf-8", "utf-8", src);
    TEST_TRUE(destPtr != NULL);
    LOG_DEBUG("converted: " << destPtr);
    char *testSrc = CharSetConvert("utf-8", "utf-8", destPtr);
    int ret = strcmp(src, testSrc);
    TEST_EQUAL(ret, 0);

    delete[] destPtr;
    delete[] testSrc;

    return true;
}
ADD_CASE(ConverterTest);

bool CharSetDectionTest(void)
{
    char src[] = "中文测试";
    const char *charset = GetCharSet(src);
    TEST_TRUE(charset != NULL);
    LOG_DEBUG("Charset is: " << charset);
    return true;
}
ADD_CASE(CharSetDectionTest);

REGISTER_UT(UnicodeUT);
