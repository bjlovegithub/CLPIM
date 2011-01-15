/**
 * SubscriptionViewer is used to display subscription info.
 * User can select one subscription's item and view its content
 * by ContentViewer.
 */

#ifndef SUBSCRIPTION_VIEWER_H
#define SUBSCRIPTION_VIEWER_H

#include <map>
#include <string>

class SubscriptionViewer
{
public:
    /**
     * @brief Set subscription info.
     */
    void SetSubInfo(const std::map<std::string, std::string> &);

    /**
     * @brief Run the loop
     */
    void GetIn();
private:
    std::map<std::string, std::string> mSubInfo;
};

#endif // SUBSCRIPTION_VIEWER_H
