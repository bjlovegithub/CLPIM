#include <iostream>
#include <string>

#include <unistd.h>

#include "subscription_manager.h"
#include "subscription_importer.h"
#include "Log.h"
#include "GlobalDef.h"
#include "Exceptions.h"

using namespace std;
using namespace CLPIM;

SubscriptionManager::SubscriptionManager()
{
    mSubFile = ConfPath+"/rss_subscription";
    try
    {
        FileHandler fh(mSubFile.c_str(), O_CREAT|O_RDWR);
        //mFileHandler.Open(subscriptionPath.c_str(), O_CREAT|O_RDWR);
        string name, url;
        while (fh.ReadLine(name) && (name != ""))
        {
            fh.ReadLine(url);
            mSubUrls[name] = url;
        }
        /// TODO
    }
    catch (const FileException &e)
    {
        LOG_ERROR("Exception: " << e.what());
    }
}

bool SubscriptionManager::ImportFromGoogleReader()
{
    string filePath;
    cout << "Please enter XML file which is exported from Google Reader\n";
    cin >> filePath;

    /// check file is valid or not.
    if (-1 == access(filePath.c_str(), R_OK | F_OK))
    {
        cout << "File: " << filePath << " is not existed or cannot be read\n";
        return false;
    }

    GoogleReaderImporter gImporter(filePath);
    LOG_DEBUG("Position flag: " << __FILE__ << " " << __LINE__);
    if (!gImporter.Import())
    {
        LOG_DEBUG("Import Google Reader subscription error!");
        return false;
    }

    map<string, string> ret = gImporter.GetUrlSet();
    mSubUrls.insert(ret.begin(), ret.end());

    return true;
}

const map<string, string>& SubscriptionManager::GetSubscriptions()
{
    return mSubUrls;
}

bool SubscriptionManager::DumpSubs()
{
    FileHandler fh(mSubFile.c_str(), O_CREAT|O_RDWR);
    typeof(mSubUrls.begin()) iter;
    for (iter = mSubUrls.begin(); iter != mSubUrls.end(); ++iter)
    {
        fh.Write(iter->first);
        fh.Write("\n");
        fh.Write(iter->second);
        fh.Write("\n");
    }

    return true;
}
