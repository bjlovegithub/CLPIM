#include "ut_tools/ut_tool.h"
#include "rdf_parser.h"
#include "xercesc_factory.h"

#include <iostream>

using namespace std;

const string RDF_PATH = "./rdf_rss.xml";

bool Test(void)
{
    XMLParserPtr parserPtr = XercescFactory::GetInst();
    parserPtr->ResetDoc(RDF_PATH);

    RDFParser p(parserPtr);
    Feed feed;
    bool ret = p.Parse(feed);
    TEST_TRUE(ret);

    TEST_EQUAL(feed.mTitle, "XML.com");
    TEST_EQUAL(feed.mUrl, "http://xml.com/pub");

    TEST_EQUAL(feed.mItems.size(), 2);
    TEST_EQUAL(feed.mItems[0].mTitle, "Processing Inclusions with XSLT");
    TEST_EQUAL(feed.mItems[0].mUrl, "http://xml.com/pub/2000/08/09/xslt/xslt.html");
    TEST_EQUAL(feed.mItems[1].mTitle, "Putting RDF to Work");

    return true;
}
ADD_CASE(Test);

REGISTER_UT(RDF_Parser_UT);

