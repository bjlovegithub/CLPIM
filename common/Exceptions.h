/**
 * This file defines exceptions used in CLPIM project.
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

namespace CLPIM
{

/**
 * Exception base for all other exceptions.
 */
class ExceptionBase : public std::exception
{
public:
    ExceptionBase(std::string msg);
    const char* what() const throw();
    ~ExceptionBase() throw();
private:
    std::string mMessage;
};

/**
 * Memory related exceptions.
 */
class MemException : public ExceptionBase
{
public:
    MemException(std::string msg);
};

/**
 * File related exceptions.
 */
class FileException : public ExceptionBase
{
public:
    FileException(std::string msg);
};

} // namespace CLPIM

#endif // EXCEPTIONS_H
