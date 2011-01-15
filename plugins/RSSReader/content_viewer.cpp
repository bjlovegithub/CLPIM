#include "content_viewer.h"
#include "Log.h"
#include "FeedParser/feed_parser.h"

#include <string>

using namespace std;
using namespace CLPIM;

ContentViewer::ContentViewer()
{
    mIsDir = true;
}

void ContentViewer::SetUrl(const string &url)
{
    mUrl = url;
}

void ContentViewer::SetTitle(const string &title)
{
    mTitle = title;
}

void ContentViewer::View()
{
    /// article id
    size_t id;
    FeedParser fp;
    fp.SetSource(mUrl, true);
    /// parse feed info from html
    if (!fp.Parse(mFeedInfo))
    {
        LOG_ERROR("FeedParser parse " << mTitle << " error!");
        cout << "Parse Error!" << endl;
        return;
    }
    const vector<FeedItem> &items = mFeedInfo.mItems;

    while (true)
    {
        cout << "[" << mTitle << "]";
        if (mIsDir) /// show article list
        {
            cout << "Show item list?[Y/N]" << endl;
            char c;
            cin >> c;
            if ('Y' == c)
            {
                /// show feed items.
                for (size_t i = 0; i < items.size(); ++i)
                {
                    cout << i+1 << ": " << items[i].mTitle;
                    cout << " Author: " << items[i].mAuthor << endl;
                    cout << items[i].mContent << endl;
                    cout << "URL: " << items[i].mUrl << endl;
                }
            }
            /// get item id
            while (true)
            {
                cout << "Enter item ID to view article content: ";
                cin >> id;
                if (id < 0 || id > items.size())
                    continue;
                else
                    break;
            }
            mIsDir = false;
        }
        else /// view article content.
        {
            const FeedItem &item = items[id];
            cout << "Title: " << item.mTitle << endl;
            cout << "Author: " << item.mAuthor 
                 << "Pub Date: " << item.mPubDate << endl;
            cout << "Url: " << item.mUrl << endl;
            cout << item.mContent << endl;

            mIsDir = true;
        }
        cout << "Enter 0 to quit" << endl;
        string cmd;
        cin >> cmd;
        if ("0" == cmd)
            break;
    }
}
