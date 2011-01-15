#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>

using namespace std;

int main(void)
{
    const char *HostName = "http://im.qq.com/";
    struct hostent *hostPtr;

    if (NULL == (hostPtr = gethostbyname(HostName)))
    {
        cout << "Get host by name error" << endl;
        perror("gethostbyname error");
        return 1;
    }
    cout << "Normalized Host Name: " << hostPtr->h_name << endl;

    char **ipList;
    char ip[32];
    switch(hostPtr->h_addrtype)
    {
    case AF_INET:
    case AF_INET6:
        ipList = hostPtr->h_addr_list;
        for (; *ipList != NULL; ++ipList)
        {
            cout << "Raw: " << *ipList << endl;
            cout << "Converted: " 
                 << inet_ntop(hostPtr->h_addrtype, ipList, ip, sizeof(ip))
                 << endl;
        }
        break;
    default:
        cout << "Unkown protocl" << endl;
    }

    return 0;
}
