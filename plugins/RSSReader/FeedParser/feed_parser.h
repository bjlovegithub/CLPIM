#ifndef FEED_PARSER_H
#define FEED_PARSER_H

#include <string>
#include "xercesc_factory.h"

/**
 * FeedItem holds each item's meta info.
 */
struct FeedItem
{
    std::string mAuthor;
    std::string mTitle;
    std::string mContent;
    uint32_t year, month, day, hour, minute, second;
    std::string mPubDate;
    std::string mUrl;
};

/**
 * Feed holds the parsed data(meta data) of feeds.
 */
struct Feed
{
    std::string mUrl;
    std::string mTitle;
    std::vector<FeedItem> mItems;
    std::string mDescription;

#ifdef DEBUG
    void Dump();
#endif
};

/**
 * Feed parser parses input file and get items.
 * It can judge input document type, and supported type is: RSS, ATOM, RDF.
 * Usage:
 *   1. Call @SetSource.  2. Call @Parse.   3. Call @GetFeed.
 */
class FeedParser
{
private:
    enum FeedType {RSS, ATOM, RDF, UNKOWN_RSS_TYPE};
public:
    /**
     * Contructor, initliaze xercesc
     */
    FeedParser();

    /**
     * @brief Set the data source for parser
     * @param source HTML file path or URL string.
     * @param isUrl Indicating source string is url or path.
     * @return Return true if set successfully, otherwise
     *         return false.
     */
    bool SetSource(const std::string &source, bool isUrl = false);
    
    /**
     * @brief Parse the feed.
     */
    bool Parse(Feed &f);

    /**
     * @brief Get parsed result.
     */
    void GetFeed(Feed &feed);

protected:
    /**
     * Get the type of input document(feed).
     * Supported feed types are: ATOM, RSS, RDF.
     */
    FeedType GetFeedType();

private:
    /// file path for the to be parsed file.
    std::string mURI;
    FeedType mFeedType;
    CLPIM::SharedPtr<XercescWrapper> mParserPtr;
    XMLCh *TAG_RDFFlag;
    XMLCh *TAG_ATOMFlag;
    XMLCh *TAG_RSSFlag;
};

#endif // FEED_PARSER_H
