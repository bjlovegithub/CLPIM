#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>
using namespace std;

XERCES_CPP_NAMESPACE_USE

XMLCh *TAG_Outline;
XMLCh *ATTR_Title;
XMLCh *ATTR_Url;

int main (int argc, char* args[]) {

    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Error during initialization! :\n"
             << message << "\n";
        XMLString::release(&message);
        return 1;
    }

    TAG_Outline = XMLString::transcode("outline");
    ATTR_Title = XMLString::transcode("title");
    ATTR_Url = XMLString::transcode("xmlUrl");

    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    char* xmlFile = "google.xml";

    try {
        parser->parse(xmlFile);
        DOMDocument* xmlDoc = parser->getDocument();
        DOMNodeList *children = xmlDoc->getElementsByTagName(TAG_Outline);
        const  XMLSize_t nodeCount = children->getLength();
        cout << nodeCount << endl;
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
                    const XMLCh* name = currElement->getAttribute(ATTR_Title);
                    cout << XMLString::transcode(name) << endl;
                }
            }
        } /// for
    }
    catch (const XMLException& toCatch) {
        char* message = XMLString::transcode(toCatch.getMessage());
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (const DOMException& toCatch) {
        char* message = XMLString::transcode(toCatch.msg);
        cout << "Exception message is: \n"
             << message << "\n";
        XMLString::release(&message);
        return -1;
    }
    catch (...) {
        cout << "Unexpected Exception \n" ;
        return -1;
    }

    delete parser;
    delete errHandler;
    return 0;
}
          
