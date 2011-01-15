#include "ut_tools/ut_tool.h"
#include "atom_parser.h"
#include "xercesc_factory.h"

#include <iostream>

using namespace std;

const string RDF_PATH = "./atom_feed.xml";

bool Test(void)
{
    XMLParserPtr parserPtr = XercescFactory::GetInst();
    parserPtr->ResetDoc(RDF_PATH);

    ATOMParser p(parserPtr);
    Feed feed;
    bool ret = p.Parse(feed);
    TEST_TRUE(ret);

    TEST_EQUAL(feed.mTitle, "dive into mark");

    TEST_EQUAL(feed.mItems.size(), 1);
    TEST_EQUAL(feed.mItems[0].mTitle, "Atom draft-07 snapshot");
    TEST_EQUAL(feed.mItems[0].mPubDate, "2003-12-13T08:29:29-04:00");
    //TEST_EQUAL(feed.mItems[0].mAuthor, "Mark Pilgrim");

    return true;
}
ADD_CASE(Test);

REGISTER_UT(ATOM_Parser_UT);

