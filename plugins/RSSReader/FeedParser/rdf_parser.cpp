#include "feed_parser.h"
#include "rdf_parser.h"
#include "Log.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace CLPIM;

RDFParser::RDFParser(SharedPtr<XercescWrapper> ptr) : mParserPtr(ptr)
{
    TAG_CHANNELFlag = XMLString::transcode("channel");
    TAG_TITLEFlag = XMLString::transcode("title");
    TAG_LINKFlag = XMLString::transcode("link");
    TAG_DESCFlag = XMLString::transcode("description");
    TAG_ITEMFlag = XMLString::transcode("item");
}

bool RDFParser::Parse(Feed &feed)
{
    /// TODO
    vector<string> ret;
    /// Get Channle title, description, etc.
    if (!mParserPtr->GetTagValue(TAG_CHANNELFlag, ret))
    {
        LOG_INFO("Cannot find channel");
        return true;
    }

    {
        ret.clear();
        mParserPtr->GetTagValue(TAG_TITLEFlag, ret);
        feed.mTitle = ret[0];
        
        ret.clear();
        mParserPtr->GetTagValue(TAG_LINKFlag, ret);
        feed.mUrl = ret[0];
        
        ret.clear();
        mParserPtr->GetTagValue(TAG_DESCFlag, ret);
        feed.mDescription = ret[0];
    }

    /// Get items
    vector<XMLCh*> subTags;
    subTags.push_back(TAG_TITLEFlag);
    subTags.push_back(TAG_LINKFlag);
    subTags.push_back(TAG_DESCFlag);

    vector<vector<string> > rets;
    if (!mParserPtr->GetTagValueByTag(TAG_ITEMFlag, subTags, rets))
    {
        LOG_INFO("Parse items error");
        return true;
    }

    {
        for (size_t i = 0; i < rets.size(); ++i)
        {
            FeedItem item;
            item.mTitle = rets[i][0];
            item.mUrl = rets[i][1];
            item.mContent = rets[i][2];
            feed.mItems.push_back(item);
        }
    }

    return true;
}
