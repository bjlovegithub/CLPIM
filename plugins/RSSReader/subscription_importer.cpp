#include "subscription_importer.h"
#include "Log.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

using namespace std;

XERCES_CPP_NAMESPACE_USE

GoogleReaderImporter::GoogleReaderImporter(const string &path) : mPath(path)
{}

bool GoogleReaderImporter::Import()
{
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        LOG_DEBUG("Error during initialization! :\n" << message << "\n");
        XMLString::release(&message);
        return false;
    }

    XMLCh *TAG_Outline = XMLString::transcode("outline");
    XMLCh *ATTR_Title = XMLString::transcode("title");
    XMLCh *ATTR_Url = XMLString::transcode("xmlUrl");

    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    LOG_DEBUG("Position flag in " << __FILE__ << " " << __LINE__);

    try
    {
        parser->parse(mPath.c_str());
        DOMDocument* xmlDoc = parser->getDocument();
        DOMNodeList *children = xmlDoc->getElementsByTagName(TAG_Outline);
        const  XMLSize_t nodeCount = children->getLength();
        LOG_DEBUG("Node Num: " << nodeCount);
        for (XMLSize_t i = 0; i < nodeCount; ++i)
        {
            DOMNode *currNode = children->item(i);
            if (currNode->getNodeType() && 
                currNode->getNodeType() == DOMNode::ELEMENT_NODE)
            {
                DOMElement *currElement =
                    dynamic_cast<xercesc::DOMElement*>(currNode);
                if(XMLString::equals(currElement->getTagName(), TAG_Outline))
                {
                    string name =
                        XMLString::transcode(currElement->getAttribute(ATTR_Title));
                    LOG_DEBUG("Subscription Title: " << name);
                    string url =
                        XMLString::transcode(currElement->getAttribute(ATTR_Url));
                    LOG_DEBUG("Subscription Url: " << url);
                    if ("" != name && "" != url)
                        mUrlSet[name] = url;
                }
            }
        } /// for
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        LOG_DEBUG("Exception message is: \n" << message << "\n");
        XMLString::release(&message);
        return false;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        LOG_DEBUG("Exception message is: \n" << message << "\n");
        XMLString::release(&message);
        return false;
    }
    catch (...) {
        LOG_DEBUG("Unexpected Exception \n");
        return false;
    }

    delete parser;
    delete errHandler;
    return true;
}

map<string, string> GoogleReaderImporter::GetUrlSet()
{
    return mUrlSet;
}
