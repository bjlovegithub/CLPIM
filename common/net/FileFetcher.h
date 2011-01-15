/**
 * FileFetcher is reponsible fetch file from remote server.
 * It wrapps socket interface.
 */

#ifndef FILE_FETCHER_H
#define FILE_FETCHER_H

#include <string>
#include <vector>

/// For Unittest
bool TestGetHostname(void);
bool TestGetHostIP(void);

namespace CLPIM
{

enum ProtocolType {HTTP, FTP, UNKOWN};
struct HostInfo
{
    ProtocolType mProtocolType;
    int mType;
    std::vector<std::string> mIPSet;
    std::vector<std::string> mParsedIPSet; // parsed by inet_ntop from mIPSet.
    std::string mFilePath;
    std::string mHostName;
    int mHostPort;
    std::string mUrl;  // full url
};


class FileFetcher
{
/// For Unittest
friend bool ::TestGetHostname(void);
friend bool ::TestGetHostIP(void);

public:
    /**
     * Constructor
     */
    FileFetcher();

    /**
     * Destructor
     */
    ~FileFetcher();


    /**
     * Read http page info from @url.
     * @param url Source URL.
     * @param receivedMsg Received http content from remote server.
     * @return Return true for success, false for failure.
     */
    bool ReadHTTPFiles(const std::string &url, std::string &receivedMsg);

private:
    /**
     * Get host's ip addresses.
     * @para info Host info including host name.
     * @return True for success, false for failure.
     */
    bool GetHostIP(HostInfo &info);

    /**
     * Strip other things from full url, and get hostname only.
     * @return If url is valid and parse successfully, return true.
     *         Otherwise, return false.
     */
    bool GetHostname(HostInfo &info);

    /**
     * From HTTP GET Request Command.
     * @param info @HostInfo contains url info.
     * @return HTTP GET request command.
     */
    std::string FormHTTPGetRequest(const HostInfo &info);

private:
    char *mBuffer;
};

} // namespace CLPIM

#endif // FILE_FETCHER_H
