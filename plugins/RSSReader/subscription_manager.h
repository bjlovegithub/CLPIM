/**
 * SubscriptionManager manages subscriptions.
 * There is a backend to hold subscriptions.
 */

#ifndef SUBSCRIPTION_MANAGER_H
#define SUBSCRIPTION_MANAGER_H

#include <string>
#include <map>

#include "FileUtil.h"

class SubscriptionManager
{
public:
    /**
     * Constructor. Read subscriptions from backend.
     */
    SubscriptionManager();

    /**
     * Import subscritpions from xml file imported from Google Reader.
     * @return Return true if imported successfully, otherwise false.
     */
    bool ImportFromGoogleReader();

    /**
     * Get subscriptions
     */
    const std::map<std::string, std::string>& GetSubscriptions();

    /**
     * Dump subscription info into file.
     */
    bool DumpSubs();

private:
    /// Subscription's url set
    std::map<std::string, std::string> mSubUrls;
    /// File handler to read and write subscription info.
    CLPIM::FileHandler mFileHandler;
    std::string mSubFile;
    
};

#endif // SUBSCRIPTION_MANAGER_H
