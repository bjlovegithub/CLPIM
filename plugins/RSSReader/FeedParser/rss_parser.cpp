#include "rss_parser.h"
#include "Log.h"

#include <string>
#include <vector>

using namespace std;
using namespace CLPIM;

RSSParser::RSSParser(SharedPtr<XercescWrapper> ptr) : mParserPtr(ptr)
{
    TAG_TITLEFlag = XMLString::transcode("title");
    TAG_LINKFlag = XMLString::transcode("link");
    TAG_DESCFlag = XMLString::transcode("description");
    TAG_CHANNELFlag = XMLString::transcode("channel");
    TAG_ITEMFlag = XMLString::transcode("item");
    TAG_PUBLISHEDFlag = XMLString::transcode("pubDate");
    TAG_AUTHORDFlag = XMLString::transcode("author");
}

bool RSSParser::Parse(Feed &feed)
{
    vector<vector<string> > rets;
    vector<XMLCh*> subTags;
    subTags.push_back(TAG_TITLEFlag);
    subTags.push_back(TAG_DESCFlag);
    subTags.push_back(TAG_LINKFlag);

    /// Get channel info
    if (!mParserPtr->GetTagValueByTag(TAG_CHANNELFlag, subTags, rets))
        LOG_INFO("Get RSS meta info return 0");
    else
    {
        feed.mTitle = rets[0][0];
        feed.mDescription = rets[0][1];
        feed.mUrl = rets[0][2];
    }

    /// get entries
    rets.clear();
    subTags.clear();

    subTags.push_back(TAG_TITLEFlag);
    subTags.push_back(TAG_LINKFlag);
    subTags.push_back(TAG_AUTHORDFlag);
    subTags.push_back(TAG_PUBLISHEDFlag);
    subTags.push_back(TAG_DESCFlag);

    if (!mParserPtr->GetTagValueByTag(TAG_ITEMFlag, subTags, rets))
    {
        LOG_INFO("Cannot Parse Items in RSS");
        return true;
    }

    for (size_t i = 0; i < rets.size(); ++i)
    {
        FeedItem item;
        item.mTitle = rets[i][0];
        item.mUrl = rets[i][1];
        item.mAuthor = rets[i][2];
        item.mPubDate = rets[i][3];
        item.mContent = rets[i][4];
        feed.mItems.push_back(item);
    }

    return true;
}
