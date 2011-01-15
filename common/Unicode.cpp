#include "Unicode.h"
#include "Log.h"

#include <langinfo.h>
#include <iconv.h>
#include <errno.h>
#include <string.h>
#include <exception>

using namespace std;

static const size_t CHAR_SET_NAME_LENGTH = 10;
static const char SUPPORTED_CHAR_SET[][CHAR_SET_NAME_LENGTH] = 
{"gbk", "utf-8", "gb2312"};

char* CLPIM::CharSetConvert(const char *to, const char *from, char *src)
{
    iconv_t conv = iconv_open(to, from);
    if ((iconv_t)(-1) == conv)
    {
        LOG_ERROR("iconv_open error: " << strerror(errno));
        return NULL;
    }

    size_t srcLen = strlen(src);
    size_t destLen = 4*srcLen;
    char *destPtr = new (nothrow) char[destLen];
    char *tmpPtr = destPtr;
    if (NULL == destPtr)
    {
        LOG_ERROR("Allocate(new) memory error, no enough space");
        return NULL;
    }

    memset(destPtr, 0, destLen);
    if ((size_t)(-1) == iconv(conv, &src, &srcLen, &tmpPtr, &destLen))
    {
        LOG_ERROR("iconv convert error: " << strerror(errno));
        return NULL;
    }

    iconv_close(conv);

    return destPtr;
}

char* CLPIM::InternalToLocal(char *src)
{
    char *localCharSet = nl_langinfo(CODESET);
    return CLPIM::CharSetConvert(INTERNAL_CHAR_SET, localCharSet, src);
}

char* CLPIM::LocalToInternal(char *src)
{
    char *localCharSet = nl_langinfo(CODESET);
    return CLPIM::CharSetConvert(localCharSet, INTERNAL_CHAR_SET, src);
}

const char* CLPIM::GetCharSet(char *src)
{
    size_t charsetNum = sizeof(SUPPORTED_CHAR_SET)/CHAR_SET_NAME_LENGTH;
    size_t i;
    for (i = 0; i < charsetNum; ++i)
    {
        const char *from = SUPPORTED_CHAR_SET[i];
        const char *to = INTERNAL_CHAR_SET;
        char* destPtr = CharSetConvert(to, from, src);
        if (NULL == destPtr)
        {
            LOG_INFO("Input string char set is not " << from);
            delete[] destPtr;
        }
        else
        {
            delete[] destPtr;
            break;
        }
    }

    if (i == charsetNum)
        return NULL;

    return SUPPORTED_CHAR_SET[i];
}
