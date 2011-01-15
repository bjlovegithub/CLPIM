#include "atom_parser.h"
#include "Log.h"

#include <string>
#include <vector>

using namespace std;
using namespace CLPIM;

ATOMParser::ATOMParser(SharedPtr<XercescWrapper> ptr) : mParserPtr(ptr)
{
    TAG_TITLEFlag = XMLString::transcode("title");
    TAG_LINKFlag = XMLString::transcode("link");
    TAG_DESCFlag = XMLString::transcode("subtitle");
    TAG_ENTRYFlag = XMLString::transcode("entry");
    TAG_UPDATEFlag = XMLString::transcode("updated");
    TAG_PUBLISHEDFlag = XMLString::transcode("published");
    TAG_AUTHORDFlag = XMLString::transcode("author");
    TAG_CONTENTFlag = XMLString::transcode("content");
}

bool ATOMParser::Parse(Feed &feed)
{
    LOG_DEBUG("Start Parsing...");
    vector<string> ret;
    mParserPtr->GetTagValue(TAG_TITLEFlag, ret);
    feed.mTitle = ret[0];

    ret.clear();
    mParserPtr->GetTagValue(TAG_DESCFlag, ret);
    feed.mDescription = ret[0];

    /// get entries
    vector<XMLCh*> subTags;
    subTags.push_back(TAG_TITLEFlag);
    subTags.push_back(TAG_LINKFlag);
    subTags.push_back(TAG_AUTHORDFlag);
    subTags.push_back(TAG_PUBLISHEDFlag);
    subTags.push_back(TAG_CONTENTFlag);

    vector<vector<string> > rets;
    if (!mParserPtr->GetTagValueByTag(TAG_ENTRYFlag, subTags, rets))
    {
        LOG_INFO("Cannot Parse Entries in ATOM");
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
