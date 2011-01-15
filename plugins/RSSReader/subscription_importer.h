#ifndef SUBSCRIPTION_IMPORTER_H
#define SUBSCRIPTION_IMPORTER_H

#include <map>
#include <string>

class GoogleReaderImporter
{
public:
    GoogleReaderImporter(const std::string &path);
    bool Import();
    std::map<std::string, std::string> GetUrlSet();
private:
    std::map<std::string, std::string> mUrlSet;
    std::string mPath;
};

#endif /// SUBSCRIPTION_IMPORTER_H
