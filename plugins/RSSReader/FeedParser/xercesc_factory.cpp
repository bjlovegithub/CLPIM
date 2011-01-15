#include "xercesc_factory.h"
#include "Log.h"

#include <fstream>

using namespace std;
using namespace CLPIM;

/********* Implementation for XercescWrapper **********/
XercescWrapper::XercescWrapper() : mXMLDoc(0)
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException &e)
    {
        char* message = XMLString::transcode(e.getMessage());
        LOG_ERROR("Init xercesc error!\n" << message);
        XMLString::release(&message);
    }

    mParser = new XercesDOMParser();
    mParser->setValidationScheme(XercesDOMParser::Val_Always);
    mParser->setDoNamespaces(true);

    mErrorHandler = (ErrorHandler*)(new HandlerBase);
    mParser->setErrorHandler(mErrorHandler);
}

XercescWrapper::~XercescWrapper()
{
    delete mParser;
    delete mErrorHandler;
}

bool XercescWrapper::ResetDoc(const string &path)
{
    mFilePath = path;
    mFileContent = "";

/*
    ifstream ifs(path.c_str(), ifstream::in);
    char c;
    while (ifs >> c)
        mFileContent += c;
*/
    /// Re-parse file.
    try
    {
        mParser->parse(mFilePath.c_str());
        /// TODO: Needed to delete mXMLDoc first?
        mXMLDoc = mParser->getDocument();
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        LOG_ERROR("XMLException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const DOMException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.msg);
        LOG_ERROR("DOMException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const SAXException &e)
    {
        char* message = XMLString::transcode(e.getMessage());
        LOG_ERROR("SAXException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (...)
    {
        LOG_ERROR("Unexpected Exception");
        return false;
    }

    return true;
}

bool XercescWrapper::CheckTag(XMLCh *tag)
{
    try
    {
        if (!mXMLDoc)
            return false;
        DOMNodeList *list = mXMLDoc->getElementsByTagName(tag);
        const XMLSize_t c = list->getLength();
        if (c < 1)
            return false;
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        LOG_ERROR("XMLException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const DOMException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.msg);
        LOG_ERROR("DOMException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const SAXException &e)
    {
        char* message = XMLString::transcode(e.getMessage());
        LOG_ERROR("SAXException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (...)
    {
        LOG_ERROR("Unexpected Exception");
        return false;
    }

    return true;
}

bool XercescWrapper::GetTagValue(XMLCh *tag, vector<string> &ret)
{
    try
    {
        DOMNodeList *list = mXMLDoc->getElementsByTagName(tag);
        const XMLSize_t c = list->getLength();
        if (c > 0)
        {
            for (XMLSize_t i = 0; i < c; ++i)
            {
                DOMElement *e =
                    dynamic_cast<DOMElement*>(list->item(i));
                ret.push_back(XMLString::transcode(e->getTextContent()));
            }
        }
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        LOG_ERROR("XMLException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const DOMException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.msg);
        LOG_ERROR("DOMException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const SAXException &e)
    {
        char* message = XMLString::transcode(e.getMessage());
        LOG_ERROR("SAXException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (...)
    {
        LOG_ERROR("Unexpected Exception");
        return false;
    }

    return true;
}

bool XercescWrapper::GetTagValueByTag(XMLCh *pTag, vector<XMLCh*> &subTags,
                                      vector<vector<string> > &ret)
{
    try
    {
        DOMNodeList *pList = mXMLDoc->getElementsByTagName(pTag);
        const XMLSize_t c = pList->getLength();
        if (c < 1)
            return false;
        for (XMLSize_t i = 0; i < c; ++i)
        {
            vector<string> v;
            DOMElement *e = dynamic_cast<DOMElement*>(pList->item(i));
            for (size_t j = 0; j < subTags.size(); ++j)
            {
                DOMNodeList *list = e->getElementsByTagName(subTags[j]);
                if (list->getLength() < 1)
                {
                    v.push_back(string(""));
                    continue;
                }
                DOMElement *tmpEle = dynamic_cast<DOMElement*>(list->item(0));
                v.push_back(XMLString::transcode(tmpEle->getTextContent()));
            }
            ret.push_back(v);
        }
    }
    catch (const XMLException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.getMessage());
        LOG_ERROR("XMLException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const DOMException& toCatch)
    {
        char* message = XMLString::transcode(toCatch.msg);
        LOG_ERROR("DOMException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (const SAXException &e)
    {
        char* message = XMLString::transcode(e.getMessage());
        LOG_ERROR("SAXException: " << message);
        XMLString::release(&message);
        return false;
    }
    catch (...)
    {
        LOG_ERROR("Unexpected Exception");
        return false;
    }

    return true;
}


/****** Implementation for XercescFactory ******/
SharedPtr<XercescWrapper> XercescFactory::sInst;

SharedPtr<XercescWrapper> XercescFactory::GetInst()
{
    if (!sInst)
    {
        sInst.Reset(new XercescWrapper);
    }
    return sInst;
}

