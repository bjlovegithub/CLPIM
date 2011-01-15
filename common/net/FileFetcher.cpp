#include "FileFetcher.h"
#include "Log.h"
#include "Exceptions.h"

#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace CLPIM;

#define HTTP_PORT 80
#define MAX_BUFFER_SIZE 2048 /// 2K

FileFetcher::FileFetcher()
{
    try
    {
        mBuffer = new char[MAX_BUFFER_SIZE];
        if (NULL == mBuffer)
        {
            LOG_FATAL("Allocate buffer error!");
            throw MemException("new operator return null");
        }
    }
    catch (const exception &e)
    {
        throw e;
    }
}

FileFetcher::~FileFetcher()
{
    delete[] mBuffer;
}

bool FileFetcher::ReadHTTPFiles(const string &url, string &receivedMsg)
{
    LOG_DEBUG("Call Trace");
    HostInfo info;
    info.mUrl = url;

    if (!GetHostname(info))
    {
        LOG_ERROR("GetHostname Error!");
        return false;
    }

    if (!GetHostIP(info))
    {
        LOG_ERROR("GetHostIP return zero ips");
        return false;
    }

    struct sockaddr_in remoteHost;
    bzero(&remoteHost, sizeof(remoteHost));
    remoteHost.sin_family = AF_INET;
    remoteHost.sin_port = htons(HTTP_PORT);
    remoteHost.sin_addr = *((struct in_addr*)info.mIPSet[0].c_str());
    LOG_DEBUG("ip in int: " << *(int*)(info.mIPSet[0].c_str()));
    LOG_DEBUG("ip in remoteHost: " << remoteHost.sin_addr.s_addr);
    //memcpy(&remoteHost.sin_addr.s_addr, info.mIPSet[0].c_str(),
    //       info.mIPSet[0].size());

    int remoteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == remoteSocket)
    {
        LOG_ERROR("Create socket error: " << strerror(errno));
        return false;
    }

    LOG_DEBUG("Socket creation succeeds" << info.mIPSet[0]);
    if (-1 == connect(remoteSocket, (struct sockaddr*)&remoteHost,
                      sizeof(remoteHost)))
    {
        LOG_ERROR("Connect socket error: " << strerror(errno));
        return false;
    }
    LOG_DEBUG("Socket connection is Ok");
    string req = FormHTTPGetRequest(info);
    {
        size_t sentBytes, totalBytes;
        totalBytes = req.size();
        sentBytes = 0;
        while (sentBytes < totalBytes)
        {
            int tmp = write(remoteSocket, req.substr(sentBytes).c_str(),
                            totalBytes-sentBytes);
            if (-1 == tmp)
            {
                LOG_ERROR("Send GET request error: " << strerror(errno));
                return false;
            }
            else
                sentBytes += tmp;
        }
    }
    LOG_DEBUG("Send request succeed");
    receivedMsg = "";
    memset(mBuffer, 0, MAX_BUFFER_SIZE);
    while (read(remoteSocket, mBuffer, MAX_BUFFER_SIZE-1) > 0)
    {
        receivedMsg += mBuffer;
        memset(mBuffer, 0, MAX_BUFFER_SIZE);
    }

    LOG_DEBUG("Receive message successfully");
    if (-1 == close(remoteSocket))
    {
        LOG_ERROR("Socket shut down error" << strerror(errno));
        return false;
    }
    LOG_DEBUG("Finish socket");

    return true;
}

bool FileFetcher::GetHostIP(HostInfo &info)
{
    struct hostent *hostPtr;
    const string host = info.mHostName;
    vector<string> &ret = info.mIPSet;
    vector<string> &parsedRet = info.mParsedIPSet;

    if (NULL == (hostPtr = gethostbyname(host.c_str())))
    {
        LOG_ERROR("gethostbyname return NULL. " << strerror(errno));
        return false;
    }
    LOG_INFO("Normalized Host Name: " << hostPtr->h_name);

    char **ipList;
    char ip[256];
    switch(hostPtr->h_addrtype)
    {
    case AF_INET:
    case AF_INET6:
        info.mType = hostPtr->h_addrtype;
        ipList = hostPtr->h_addr_list;
        for (; *ipList != NULL; ++ipList)
        {
            memset(ip, 0, sizeof(ip));
            LOG_DEBUG("ip in int: " << *(int*)(*ipList));
            if (NULL == inet_ntop(hostPtr->h_addrtype, ipList, ip, sizeof(ip)))
                LOG_ERROR("inet_ntop return NULL");
            else
            {
                ret.push_back(*ipList);
                parsedRet.push_back(ip);
            }
        }
        break;
    default:
        LOG_ERROR("gethostbyname return UNKOWN protocol, not ipv4 or ipv6");
        return false;
    }

    return true;
}

bool FileFetcher::GetHostname(HostInfo &info)
{
    const string &url = info.mUrl;
    LOG_DEBUG(url);
    if (url == "")
        return false;

    info.mType = UNKOWN;
    /// parse protocol type
    if (url.find("http://") != string::npos)
    {
        info.mProtocolType = HTTP;
        info.mHostPort = 80;
    }
    else if (url.find("ftp://") != string::npos)
    {
        info.mType = FTP;
        info.mHostPort = 21;
    }
    else
    {
        /// TODO: Add more protocols here.
    }

    /// parse hostname
    size_t pos = url.find("://");
    if (pos == string::npos)
        pos = 0;
    else
        pos += 3;
    size_t endPos = url.find("/", pos);
    if (endPos == string::npos)
        endPos = url.length()-1;
    else
        endPos -= 1;

    if (endPos == pos)
        return false;

    info.mHostName = url.substr(pos, endPos-pos+1);

    if (endPos+2 >= url.length())
        info.mFilePath = "";
    else
    {
        endPos += 2;
        info.mFilePath = url.substr(endPos, url.length()-endPos);
    }

    return true;
}

string FileFetcher::FormHTTPGetRequest(const HostInfo &info)
{
    char buf[1024];
    sprintf(buf, "GET /%s HTTP/1.1\r\nAccept: */*\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n"
            "Host: %s\r\nConnection: Close\r\n\r\n",
            info.mFilePath.c_str(), info.mHostName.c_str());

    string request(buf);
    LOG_DEBUG(request);
    return request;
}
