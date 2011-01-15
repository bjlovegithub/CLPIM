#include "feed_parser.h"
#include "Log.h"
#include "net/FileFetcher.h"
#include "FileUtil.h"
#include "rss_parser.h"
#include "rdf_parser.h"
#include "atom_parser.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>
#include <string>

using namespace std;
using namespace CLPIM;
XERCES_CPP_NAMESPACE_USE

FeedParser::FeedParser()
{
    mParserPtr = XercescFactory::GetInst();
    TAG_RDFFlag = XMLString::transcode("rdf:RDF");
    TAG_ATOMFlag = XMLString::transcode("feed");
    TAG_RSSFlag = XMLString::transcode("rss");
}

bool FeedParser::SetSource(const string &source, bool isUrl)
{
    if (isUrl) /// need to fetch html
    {
        /// TODO: Get html document from @source.
        FileFetcher ff;
        string html;
        if (!ff.ReadHTTPFiles(source, html))
            LOG_ERROR("ReadHTTPFiles Error");
        LOG_DEBUG("HTML: \n" << html);
        /// strip html header
        size_t pos = html.find("<?xml");
        if (pos == string::npos)
        {
            LOG_INFO("Do not find <?xml in fetched html source");
            return false;
        }
        else
            html = html.substr(pos);

        /// write html to tmp file.
        FileHandler fh("/tmp/tmp", O_CREAT|O_RDWR);
        fh.Write(html);
        mURI = "/tmp/tmp";
    }
    else
    {
        mURI = source;
    }

    mParserPtr->ResetDoc(mURI);
    LOG_DEBUG("ResetDoc successfully!");

    return true;
}

bool FeedParser::Parse(Feed &f)
{
    /// Check feed type.
    FeedType feedType = GetFeedType();
    bool ret = true;
    if (RSS == feedType)
    {
        RSSParser parser(XercescFactory::GetInst());
        if (!parser.Parse(f))
        {
            ret = false;
            LOG_ERROR("RSSParser parse error");
        }
    }
    else if (ATOM == feedType)
    {

    }
    else if (RDF == feedType)
    {
    }
    else
        ret = false;
    
    return ret;
}

FeedParser::FeedType FeedParser::GetFeedType()
{
    if (mParserPtr->CheckTag(TAG_RDFFlag))
        return RDF;
    else if (mParserPtr->CheckTag(TAG_ATOMFlag))
        return ATOM;
    else if (mParserPtr->CheckTag(TAG_RSSFlag))
        return RSS;
    return UNKOWN_RSS_TYPE;
}
