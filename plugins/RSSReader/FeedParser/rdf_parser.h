/**
 * RDF Parser is used to parse RDF formatted subscritions.
 */

#ifndef RDF_PARSER_H
#define RDF_PARSER_H

#include <shared_ptr/shared_ptr.hpp>
#include "feed_parser.h"

class RDFParser
{
public:
    /**
     * Constructor, parameter xmlParserPtr is a pointer to XercescWrapper(get
     * instance from XercescFactory)
     **/
    RDFParser(CLPIM::SharedPtr<XercescWrapper> xmlParserPtr);

    /**
     * Parse RDF formatted input file.
     * @return parameter feed store all the parsed info from input files.
     * @return return true if successfully, return false for any error.
     */
    bool Parse(Feed &feed);

private:
    CLPIM::SharedPtr<XercescWrapper> mParserPtr;
    XMLCh *TAG_CHANNELFlag;
    XMLCh *TAG_TITLEFlag;
    XMLCh *TAG_LINKFlag;
    XMLCh *TAG_DESCFlag;
    XMLCh *TAG_ITEMFlag;
};


#endif // RDF_PARSER_H
