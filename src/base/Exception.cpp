//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/Exception.h"

#include <typeinfo>

GLUE_START_NAMESPACE

Exception::Exception(int code) : m_pNested(0), m_code(code)
{}

Exception::Exception(const std::string& msg, int code) : m_msg(msg), m_pNested(0), m_code(code)
{}

Exception::Exception(const std::string& msg, const std::string& arg, int code)
    : m_msg(msg), m_pNested(0), m_code(code)
{
    if (!arg.empty())
    {
        m_msg.append(": ");
        m_msg.append(arg);
    }
}

Exception::Exception(const std::string& msg, const Exception& nested, int code)
    : m_msg(msg), m_pNested(nested.clone()), m_code(code)
{}

Exception::Exception(const Exception& exc)
    : std::exception(exc), m_msg(exc.m_msg), m_code(exc.m_code)
{
    m_pNested = exc.m_pNested ? exc.m_pNested->clone() : 0;
}

Exception::~Exception() noexcept
{
    delete m_pNested;
}

Exception& Exception::operator=(const Exception& exc)
{
    if (&exc != this)
    {
        Exception* newPNested = exc.m_pNested ? exc.m_pNested->clone() : 0;
        delete m_pNested;
        m_msg = exc.m_msg;
        m_pNested = newPNested;
        m_code = exc.m_code;
    }
    return *this;
}

const char* Exception::name() const noexcept
{
    return "Exception";
}

const char* Exception::className() const noexcept
{
    return typeid(*this).name();
}

const char* Exception::what() const noexcept
{
    return name();
}

std::string Exception::displayText() const
{
    std::string txt = name();
    if (!m_msg.empty())
    {
        txt.append(": ");
        txt.append(m_msg);
    }
    return txt;
}

void Exception::extendedMessage(const std::string& arg)
{
    if (!arg.empty())
    {
        if (!m_msg.empty()) m_msg.append(": ");
        m_msg.append(arg);
    }
}

Exception* Exception::clone() const
{
    return new Exception(*this);
}

void Exception::rethrow() const
{
    throw *this;
}

GLUE_IMPLEMENT_EXCEPTION(LogicException, Exception, "Logic exception")
GLUE_IMPLEMENT_EXCEPTION(AssertionViolationException, LogicException, "Assertion violation")
GLUE_IMPLEMENT_EXCEPTION(NullPointerException, LogicException, "Null pointer")
GLUE_IMPLEMENT_EXCEPTION(NullValueException, LogicException, "Null value")
GLUE_IMPLEMENT_EXCEPTION(BugcheckException, LogicException, "Bugcheck")
GLUE_IMPLEMENT_EXCEPTION(InvalidArgumentException, LogicException, "Invalid argument")
GLUE_IMPLEMENT_EXCEPTION(NotImplementedException, LogicException, "Not implemented")
GLUE_IMPLEMENT_EXCEPTION(RangeException, LogicException, "Out of range")
GLUE_IMPLEMENT_EXCEPTION(IllegalStateException, LogicException, "Illegal state")
GLUE_IMPLEMENT_EXCEPTION(InvalidAccessException, LogicException, "Invalid access")
GLUE_IMPLEMENT_EXCEPTION(SignalException, LogicException, "Signal received")
GLUE_IMPLEMENT_EXCEPTION(UnhandledException, LogicException, "Unhandled exception")

GLUE_IMPLEMENT_EXCEPTION(RuntimeException, Exception, "Runtime exception")
GLUE_IMPLEMENT_EXCEPTION(NotFoundException, RuntimeException, "Not found")
GLUE_IMPLEMENT_EXCEPTION(ExistsException, RuntimeException, "Exists")
GLUE_IMPLEMENT_EXCEPTION(TimeoutException, RuntimeException, "Timeout")
GLUE_IMPLEMENT_EXCEPTION(SystemException, RuntimeException, "System exception")
GLUE_IMPLEMENT_EXCEPTION(RegularExpressionException, RuntimeException,
                         "Error in regular expression")
GLUE_IMPLEMENT_EXCEPTION(LibraryLoadException, RuntimeException, "Cannot load library")
GLUE_IMPLEMENT_EXCEPTION(LibraryAlreadyLoadedException, RuntimeException, "Library already loaded")
GLUE_IMPLEMENT_EXCEPTION(NoThreadAvailableException, RuntimeException, "No thread available")
GLUE_IMPLEMENT_EXCEPTION(PropertyNotSupportedException, RuntimeException, "Property not supported")
GLUE_IMPLEMENT_EXCEPTION(PoolOverflowException, RuntimeException, "Pool overflow")
GLUE_IMPLEMENT_EXCEPTION(NoPermissionException, RuntimeException, "No permission")
GLUE_IMPLEMENT_EXCEPTION(OutOfMemoryException, RuntimeException, "Out of memory")
GLUE_IMPLEMENT_EXCEPTION(DataException, RuntimeException, "Data error")

GLUE_IMPLEMENT_EXCEPTION(DataFormatException, DataException, "Bad data format")
GLUE_IMPLEMENT_EXCEPTION(SyntaxException, DataException, "Syntax error")
GLUE_IMPLEMENT_EXCEPTION(CircularReferenceException, DataException, "Circular reference")
GLUE_IMPLEMENT_EXCEPTION(PathSyntaxException, SyntaxException, "Bad path syntax")
GLUE_IMPLEMENT_EXCEPTION(IOException, RuntimeException, "I/O error")
GLUE_IMPLEMENT_EXCEPTION(ProtocolException, IOException, "Protocol error")
GLUE_IMPLEMENT_EXCEPTION(FileException, IOException, "File access error")
GLUE_IMPLEMENT_EXCEPTION(FileExistsException, FileException, "File exists")
GLUE_IMPLEMENT_EXCEPTION(FileNotFoundException, FileException, "File not found")
GLUE_IMPLEMENT_EXCEPTION(PathNotFoundException, FileException, "Path not found")
GLUE_IMPLEMENT_EXCEPTION(FileReadOnlyException, FileException, "File is read-only")
GLUE_IMPLEMENT_EXCEPTION(FileAccessDeniedException, FileException, "Access to file denied")
GLUE_IMPLEMENT_EXCEPTION(CreateFileException, FileException, "Cannot create file")
GLUE_IMPLEMENT_EXCEPTION(OpenFileException, FileException, "Cannot open file")
GLUE_IMPLEMENT_EXCEPTION(WriteFileException, FileException, "Cannot write file")
GLUE_IMPLEMENT_EXCEPTION(ReadFileException, FileException, "Cannot read file")
GLUE_IMPLEMENT_EXCEPTION(DirectoryNotEmptyException, FileException, "Directory not empty")
GLUE_IMPLEMENT_EXCEPTION(UnknownURISchemeException, RuntimeException, "Unknown URI scheme")
GLUE_IMPLEMENT_EXCEPTION(TooManyURIRedirectsException, RuntimeException, "Too many URI redirects")
GLUE_IMPLEMENT_EXCEPTION(URISyntaxException, SyntaxException, "Bad URI syntax")

GLUE_IMPLEMENT_EXCEPTION(ApplicationException, Exception, "Application exception")
GLUE_IMPLEMENT_EXCEPTION(BadCastException, RuntimeException, "Bad cast exception")

GLUE_END_NAMESPACE
