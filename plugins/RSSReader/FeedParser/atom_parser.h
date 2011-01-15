/**
 * Atom Parser is used to parse ATOM formatted xml files.
 */

#ifndef ATOM_PARSER_H
#define ATOM_PARSER_H

#include "shared_ptr/shared_ptr.hpp"
#include "feed_parser.h"

class ATOMParser
{
public:
    /**
     * Constructor. Parameter @xmlParser is a pointer to XML Parser.
     * The parser contains the parsed xml tree, and provide node access
     * interface.
     */
    ATOMParser(CLPIM::SharedPtr<XercescWrapper> xmlParser);

    /**
     * Parse ATOM formatted input file.
     * @return parameter feed store the parsed result(title, items, etc).
     * @return return true for successfully, false for failture.
     */
    bool Parse(Feed &feed);
private:
    CLPIM::SharedPtr<XercescWrapper> mParserPtr;
    XMLCh *TAG_TITLEFlag;
    XMLCh *TAG_LINKFlag;
    XMLCh *TAG_DESCFlag;
    XMLCh *TAG_ENTRYFlag;
    XMLCh *TAG_UPDATEFlag;
    XMLCh *TAG_PUBLISHEDFlag;
    XMLCh *TAG_AUTHORDFlag;
    XMLCh *TAG_CONTENTFlag;
};

#endif // ATOM_PARSER_H
