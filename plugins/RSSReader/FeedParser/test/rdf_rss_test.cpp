#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>
#include <string>
using namespace std;

XERCES_CPP_NAMESPACE_USE

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

    XMLCh *RDF_Flag = XMLString::transcode("rdf:RDF");
    XMLCh *TAG_Title = XMLString::transcode("title");
    XMLCh *TAG_Channel = XMLString::transcode("channel");
    XMLCh *TAG_Link = XMLString::transcode("link");
    XMLCh *TAG_Desc = XMLString::transcode("description");
    XMLCh *TAG_Item = XMLString::transcode("item");
    XMLCh *ATTR_Title = XMLString::transcode("title");
    XMLCh *ATTR_Url = XMLString::transcode("xmlUrl");

    XercesDOMParser* parser = new XercesDOMParser();
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(true);    // optional

    ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
    parser->setErrorHandler(errHandler);

    char* xmlFile = "rdf_rss.xml";
    //char* xmlFile = "google.xml";

    try {
        parser->parse(xmlFile);
        DOMDocument* xmlDoc = parser->getDocument();
        /// Check it is RDF RSS or not
        DOMNodeList *children = xmlDoc->getElementsByTagName(RDF_Flag);
        const  XMLSize_t nodeCount = children->getLength();
        if (nodeCount > 0)
            cout << "RDF" << endl;
        else
            cout << "NOT RDF" << endl;

        /// Get channel title, etc.
        {
            DOMNodeList *channelList = xmlDoc->getElementsByTagName(TAG_Channel);
            const XMLSize_t c = channelList->getLength();
            if (c > 0)
            {
                DOMElement *element =
                    dynamic_cast<xercesc::DOMElement*>(channelList->item(0));
                DOMNodeList *attrList = element->getElementsByTagName(TAG_Title);
                if (attrList->getLength() > 0)
                {
                    DOMElement *tmp =
                        dynamic_cast<xercesc::DOMElement*>(attrList->item(0));
                    const XMLCh *name = tmp->getTextContent();
                    cout << XMLString::transcode(name) << endl;
                }
            }
        }

        /// Get item.
        {
            DOMNodeList *itemList = xmlDoc->getElementsByTagName(TAG_Item);
            const XMLSize_t c = itemList->getLength();
            for (XMLSize_t i = 0; i < c; ++i)
            {
                DOMElement *element =
                    dynamic_cast<xercesc::DOMElement*>(itemList->item(i));
                DOMNodeList *attrList = element->getElementsByTagName(TAG_Title);
                if (attrList->getLength() > 0)
                {
                    DOMElement *tmp =
                        dynamic_cast<xercesc::DOMElement*>(attrList->item(0));
                    const XMLCh *name = tmp->getTextContent();
                    cout << XMLString::transcode(name) << endl;
                }
            }
        }
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
    catch (const exception &e) {
        cout << "Unexpected Exception \n"
             << e.what() << endl;
        return -1;
    }

    delete parser;
    delete errHandler;
    return 0;
}
