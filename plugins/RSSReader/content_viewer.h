/**
 * ContentViewer get content from remote server and display it
 * to the user.
 */

#ifndef CONTENT_VIEWER_H
#define CONTENT_VIEWER_H

#include <string>
#include <vector>

#include "FeedParser/feed_parser.h"

class ContentViewer
{
public:

    ContentViewer();
    void SetUrl(const std::string &url);
    void SetTitle(const std::string &title);

    /**
     * Main user operation handling loop
     */
    void View();
    
private:
    std::string mUrl, mTitle;

    /// keep current subscritpion's feed item info.
    Feed mFeedInfo;

    bool mIsDir;
    char mErrorBuf[1024];
};

#endif // CONTENT_VIEWER_H
