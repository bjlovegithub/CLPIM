#include "Exceptions.h"

using namespace std;
using namespace CLPIM;

ExceptionBase::ExceptionBase(string msg)
{
    mMessage = msg;
}

ExceptionBase::~ExceptionBase() throw()
{}

const char* ExceptionBase::what() const throw()
{
    return mMessage.c_str();
}

MemException::MemException(string msg)
    : ExceptionBase(msg)
{}

FileException::FileException(string msg)
    : ExceptionBase(msg)
{}
