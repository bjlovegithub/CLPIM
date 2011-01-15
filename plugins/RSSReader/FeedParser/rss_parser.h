/**
 * RSSParser is used to parse RSS formatted xml files.
 * Supported RSS Version: 2.0
 */

#ifndef RSS_PARSER_H
#define RSS_PARSER_H

#include "shared_ptr/shared_ptr.hpp"
#include "feed_parser.h"

class RSSParser
{
public:
    /**
     * Constructor. Parameter @xmlParser is a pointer to XML Parser.
     * The parser contains the parsed xml tree, and provide node access
     * interface.
     */
    RSSParser(CLPIM::SharedPtr<XercescWrapper> xmlParser);

    /**
     * Parse RSS formatted input file.
     * @return parameter feed store the parsed result(title, items, etc).
     * @return return true for successfully, false for failture.
     */
    bool Parse(Feed &feed);
private:
    CLPIM::SharedPtr<XercescWrapper> mParserPtr;
    XMLCh *TAG_TITLEFlag;
    XMLCh *TAG_LINKFlag;
    XMLCh *TAG_DESCFlag;
    XMLCh *TAG_CHANNELFlag;
    XMLCh *TAG_ITEMFlag;
    XMLCh *TAG_PUBLISHEDFlag;
    XMLCh *TAG_AUTHORDFlag;
};

#endif // ATOM_PARSER_H
