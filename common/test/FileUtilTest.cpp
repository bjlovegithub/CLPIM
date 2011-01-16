#include "FileUtil.h"
#include "ut_tools/ut_tool.h"
#include "Log.h"

#include <string>

using namespace CLPIM;
using namespace std;

bool TestCreateFolder(void)
{
    bool ret;
    ret = FileUtil::CreateFolder("test11");
    TEST_EQUAL(ret, true);
    ret = FileUtil::CreateFolder("/tmp/test111");
    TEST_EQUAL(ret, true);

    ret = FileUtil::DeleteFolder("test11");
    TEST_EQUAL(ret, true);
    ret = FileUtil::DeleteFolder("/tmp/test111");
    TEST_EQUAL(ret, true);

    return true;
}
ADD_CASE(TestCreateFolder);

bool FileHandlerTest(void)
{
    {
        FileHandler fh("in");
        string content;
        fh.ReadWord(content);
        TEST_EQUAL(content, "this");
        content = "";
        fh.ReadLine(content);
        TEST_EQUAL(content, "is just for test..");
    }

    {
        FileHandler fh("in");
        string content;
        fh.Read(5, content);
        TEST_EQUAL(content, "this ");
        content = "";
        fh.ReadLine(content);
        TEST_EQUAL(content, "is just for test..");
    }

    {
        FileHandler fh("in");
        int count = 0;
        string content;
        while (fh.ReadWord(content) && content != "")
        {
            LOG_DEBUG("content1: " << content);
            ++count;
        }
        TEST_EQUAL(count, 3300);
    }
    
    {
        FileHandler fh("in");
        int countLine = 0;
        string content;
        while (fh.ReadLine(content) && content != "")
        {
            TEST_EQUAL(content, "this is just for test..");
            LOG_DEBUG("content2: " << content);
            ++countLine;
        }
        TEST_EQUAL(countLine, 660);
    }

    {
        FileHandler fh("in");
        string content;
        bool ret = fh.ReadWord(content);
        TEST_EQUAL(ret, true);
        TEST_EQUAL(content, "this");
        content = "";
        fh.Read(0, 4, content);
        TEST_EQUAL(content, "this");
    }

    /// test check file
    {
        bool ret = FileUtil::CheckFile("ttttt");
        TEST_EQUAL(ret, false);
        ret = FileUtil::CheckFile("FileUtilTest.cpp");
        TEST_EQUAL(ret, true);
    }

    /// open a not-existed file
    {
        FileHandler fh;
        bool ret = fh.Open("ttt", O_RDWR|O_CREAT);
        TEST_EQUAL(ret, true);
    }

    return true;
}
ADD_CASE(FileHandlerTest);

REGISTER_UT(FileUtil_UT);
