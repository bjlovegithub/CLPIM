#include "FileUtil.h"
#include "Log.h"
#include "Exceptions.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include <stdlib.h>
#include <errno.h>
#include <string.h>

using namespace std;
using namespace CLPIM;

FileHandler::FileHandler()
{
    mFD = -1;
    mBegin = mEnd = BUFFER_SIZE-1;
}

FileHandler::FileHandler(const string &fileName, int mode)
{
    mFD = open(fileName.c_str(), mode);
    chmod(fileName.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    if (-1 == mFD)
    {
        stringstream ss;
        ss << "Open file error, name: " << fileName
           << " with mode: " << mode << ", errno: " << strerror(errno);
        LOG_ERROR(ss.str());
        throw FileException(ss.str());
    }
    mBegin = mEnd = BUFFER_SIZE-1;
}

FileHandler::~FileHandler()
{
    close(mFD);
}

bool FileHandler::Open(const string &fileName, int mode)
{
    mFD = open(fileName.c_str(), mode);
    chmod(fileName.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    if (-1 == mFD)
    {
        stringstream ss;
        ss << "Open file error, name: " << fileName
           << " with mode: " << mode << ", errno: " << strerror(errno);
        LOG_ERROR(ss.str());
        return false;
    }
    mBegin = mEnd = BUFFER_SIZE-1;
    return true;
}

bool FileHandler::Write(const string &content)
{
    ssize_t ret = write(mFD, content.c_str(), content.size());
    if (-1 == ret)
    {
        LOG_ERROR("file write error: " << strerror(errno));
        return false;
    }

    return true;
}

bool FileHandler::Read(size_t offset, size_t num, std::string &content)
{
    off_t ret = lseek(mFD, offset, SEEK_SET);
    /// Invalidate mBuffer
    mBegin = mEnd = BUFFER_SIZE-1;
    if ((off_t)(-1) == ret)
    {
        LOG_ERROR("Call lseek error: " << strerror(errno));
        return false;
    }
    return Read(num, content);
}

bool FileHandler::Read(size_t num, string &content)
{
    content = "";
    while(num > 0)
    {
        /// if there is no chars in the buffer, read BUFFER_SIZE chars from file.
        if (mBegin == mEnd)
        {
            ssize_t ret = read(mFD, mBuffer, BUFFER_SIZE);
            if (0 == ret) // end of file
            {
                LOG_INFO("Reach the end of file");
                break;
            }
            if (-1 == ret)
            {
                LOG_ERROR("Call read error: " << strerror(errno));
                return false;
            }
            mBegin = 0;
            mEnd = min(size_t(BUFFER_SIZE), size_t(ret));
        }

        size_t availableChars = mEnd-mBegin;
        if (num >= availableChars)
        {
            num -= availableChars;
            content.append(mBuffer+mBegin, availableChars);
            mBegin = mEnd;
        }
        else
        {
            content.append(mBuffer+mBegin, num);
            mBegin += num;
            num = 0;
        }
    }

    return true;
}

bool FileHandler::ReadWord(string &content)
{
    content = "";
    while(true)
    {
        if (mBegin == mEnd)
        {
            ssize_t ret = read(mFD, mBuffer, BUFFER_SIZE);
            if (0 == ret) // end of file
            {
                LOG_INFO("Reach the end of file");
                break;
            }
            if (-1 == ret)
            {
                LOG_ERROR("Call read error: " << strerror(errno));
                return false;
            }
            mBegin = 0;
            mEnd = min(size_t(BUFFER_SIZE), size_t(ret));
        }
        
        while ((mBegin < mEnd) && (!IsWhiteSpace(mBuffer[mBegin])))
        {
            content.append(1, mBuffer[mBegin]);
            ++mBegin;
        }

        if (mBegin < mEnd) // indicates that meet a whitespace in mBuffer
        {
            ++mBegin; // skip whitespace
            break;
        }
    }

    return true;
}

bool FileHandler::ReadLine(string &content)
{
    content = "";
    while(true)
    {
        if (mBegin == mEnd)
        {
            ssize_t ret = read(mFD, mBuffer, BUFFER_SIZE);
            if (0 == ret) // end of file
            {
                LOG_INFO("Reach the end of file");
                break;
            }
            if (-1 == ret)
            {
                LOG_ERROR("Call read error: " << strerror(errno));
                return false;
            }
            mBegin = 0;
            mEnd = min(size_t(BUFFER_SIZE), size_t(ret));
        }
        
        while ((mBegin < mEnd) && (mBuffer[mBegin] != '\n'))
        {
            content.append(1, mBuffer[mBegin]);
            ++mBegin;
        }

        if (mBegin < mEnd) // indicates that meet a '\n' in mBuffer
        {
            ++mBegin; // skip '\n'
            break;
        }
    }

    return true;
}

bool FileHandler::Seek(size_t offset, int startPos)
{
    off_t ret = lseek(mFD, (off_t)offset, startPos);
    if (-1 == ret)
    {
        LOG_ERROR("Call lseek error: " << strerror(errno));
        return false;
    }

    /// reset mBuffer
    mBegin = mEnd;

    return true;
}

bool FileHandler::IsOk()
{
    return (mFD != -1);
}

bool FileHandler::IsWhiteSpace(char c)
{
    bool ret = false;
    switch (c)
    {
    case ' ':
    case '\t':
    case '\n':
        ret = true;
    break;
    }

    return ret;
}

///////////////////// Implementation for class FileUtil //////////////////
bool FileUtil::CreateFolder(const string &filePath)
{
    if ("" == filePath)
    {
        LOG_ERROR("File path is empty");
        return false;
    }

    int ret = mkdir(filePath.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    if (-1 == ret)
    {
        LOG_ERROR("Call mkdir error: " << strerror(errno));
        return false;
    }

    return true;
}

bool FileUtil::DeleteFolder(const string &filePath)
{
    if ("" == filePath)
    {
        LOG_ERROR("File path is empty");
        return false;
    }

    int ret = rmdir(filePath.c_str());
    if (-1 == ret)
    {
        LOG_ERROR("Call rmdir error: " << strerror(errno));
        return false;
    }

    return true;
}

bool FileUtil::CheckFile(const string &filePath)
{
    int fd = open(filePath.c_str(), O_RDONLY);
    if (-1 == fd)
        return false;
    return true;
}

const char* FileUtil::GetCWD()
{
    /// TODO
    return NULL;
}
