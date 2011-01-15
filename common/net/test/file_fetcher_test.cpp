#include "net/FileFetcher.h"
#include "ut_tools/ut_tool.h"
#include "Exceptions.h"
#include "Log.h"

#include <string>
#include <iostream>

using namespace CLPIM;
using namespace std;

bool TestGetHostname(void)
{
    try
    {
        FileFetcher f;
        HostInfo url;
        url.mUrl = "www.baidu.com";
        f.GetHostname(url);
        TEST_EQUAL(url.mHostName, "www.baidu.com");
        TEST_EQUAL(url.mFilePath, "");
        url.mUrl = "http://www.sina.com.cn/index.html";
        f.GetHostname(url);
        TEST_EQUAL(url.mHostName, "www.sina.com.cn");
        TEST_EQUAL(url.mFilePath, "index.html");
        url.mUrl = "http://google.com/code/codesearch";
        f.GetHostname(url);
        TEST_EQUAL(url.mHostName, "google.com");
        TEST_EQUAL(url.mFilePath, "code/codesearch");
        url.mUrl = "http://google.com/c";
        f.GetHostname(url);
        TEST_EQUAL(url.mHostName, "google.com");
        TEST_EQUAL(url.mFilePath, "c");
    }
    catch (const exception &e)
    {
        return false;
    }

    return true;
}
ADD_CASE(TestGetHostname);

bool TestGetHostIP(void)
{
    string host = "www.baidu.com";
    //string destIp = "144.67.108.21";
    bool flag = false;
    try
    {
        FileFetcher f;
        HostInfo info;
        info.mHostName = host;
        f.GetHostIP(info);
        vector<string> &ips = info.mParsedIPSet;
        for (size_t i = 0; i < ips.size(); ++i)
        {
            LOG_DEBUG("Parsed IP for " << host << " is: " << ips[i]);
            flag = true;
        }
    }
    catch (const exception &e)
    {
        flag = false;
    }

    return flag;
}
ADD_CASE(TestGetHostIP);

bool TestFetchFile(void)
{
    string sourceUrl = "http://www.cnbeta.com/backend.php";
    try
    {
        FileFetcher f;
        string content("");
        bool ret = f.ReadHTTPFiles(sourceUrl, content);
        LOG_DEBUG("Fetched Content: " << content);
        TEST_EQUAL(ret, true);
        TEST_TRUE(content.size() > 0);
    }
    catch (const exception &e)
    {
        return false;
    }
    return true;
}
ADD_CASE(TestFetchFile);


REGISTER_UT(FileFetcherTest);
