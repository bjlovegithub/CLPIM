/**
 * Class XercescWrapper keeps XercesDOMParser inside, and provides interface to
 * get element and its value, etc.
 * XercescFactory use sigletone pattern to manage XercescWrapper.
 */

#ifndef XERCESC_FACTORY_H
#define XERCESC_FACTORY_H

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "shared_ptr/shared_ptr.hpp"

#include <iostream>
#include <string>
#include <vector>

XERCES_CPP_NAMESPACE_USE

class XercescWrapper
{
  public:
    XercescWrapper();
    ~XercescWrapper();

    /**
     * Reset Document path.
     * Call this function will cause parser to reload document.
     */
    bool ResetDoc(const std::string &path);

    /**
     * Check @tag is in the XML or not.
     * Return true if this tag is in xml, otherwise return false.
     */
    bool CheckTag(XMLCh *tag);

    /**
     * Get @tag's content into @ret. If there is multiple tag in
     * the xml file, just return the first one content.
     * Return true on success, false for errors.
     */
    bool GetTagValue(XMLCh *tag, std::vector<std::string> &ret);

    /**
     * Get all @subTags' content, and put them into @ret. All subTags' 
     * parent tag is @pTag in XML tree.
     * Return true for success, false for failure.
     */
    bool GetTagValueByTag(XMLCh *pTag, std::vector<XMLCh*> &subTags,
                          std::vector<std::vector<std::string> > &ret);
  private:
    XercesDOMParser *mParser;
    DOMDocument *mXMLDoc;
    ErrorHandler *mErrorHandler;
    std::string mFilePath;
    std::string mFileContent;
};

typedef CLPIM::SharedPtr<XercescWrapper> XMLParserPtr;

class XercescFactory
{
public:
    static XMLParserPtr GetInst();
private:
    XercescFactory()
    {}
    XercescFactory(const XercescFactory &)
    {}
private:
    static XMLParserPtr sInst;
};

#endif // XERCESC_FACTORY_H
