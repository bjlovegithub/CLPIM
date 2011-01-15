#include "ut_tools/ut_tool.h"
#include "rss_parser.h"
#include "xercesc_factory.h"

#include <iostream>

using namespace std;

const string RDF_PATH = "rss_newssh.xml";

bool Test(void)
{
    XMLParserPtr parserPtr = XercescFactory::GetInst();
    parserPtr->ResetDoc(RDF_PATH);

    RSSParser p(parserPtr);
    Feed feed;
    bool ret = p.Parse(feed);
    TEST_TRUE(ret);

    TEST_EQUAL(feed.mTitle, "社会新闻");
    TEST_EQUAL(feed.mUrl, "http://news.qq.com/society_index.shtml");

    TEST_EQUAL(feed.mItems.size(), 20);
    TEST_EQUAL(feed.mItems[0].mTitle, "美31岁女子假扮成14岁男孩约会少女被判入狱");
    TEST_EQUAL(feed.mItems[0].mAuthor, "www.qq.com");
    TEST_EQUAL(feed.mItems[0].mPubDate, "2010-10-08 16:08:59");
    //TEST_EQUAL(feed.mItems[0].mAuthor, "Mark Pilgrim");

    return true;
}
ADD_CASE(Test);

REGISTER_UT(ATOM_Parser_UT);

