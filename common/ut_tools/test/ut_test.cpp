#include "ut_tool.h"

#include <string>
using namespace std;

bool Case1(void)
{
    TEST_EQUAL(1, 1);
}
ADD_CASE(Case1);

bool Case2(void)
{
    string src = "1";
    TEST_EQUAL(src, "2");
}
ADD_CASE(Case2);

REGISTER_UT(UT_Test);
