//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/standard_header.h"

GLUE_START_NAMESPACE

class GLUE_API Exception : public std::exception
/// This is the base class for all exceptions defined
/// in the GLUE class library.
{
public:
    Exception(const std::string& msg, int code = 0);
    /// Creates an exception.

    Exception(const std::string& msg, const std::string& arg, int code = 0);
    /// Creates an exception.

    Exception(const std::string& msg, const Exception& nested, int code = 0);
    /// Creates an exception and stores a clone
    /// of the nested exception.

    Exception(const Exception& exc);
    /// Copy constructor.

    ~Exception() noexcept;
    /// Destroys the exception and deletes the nested exception.

    Exception& operator=(const Exception& exc);
    /// Assignment operator.

    virtual const char* name() const noexcept;
    /// Returns a static string describing the exception.

    virtual const char* className() const noexcept;
    /// Returns the name of the exception class.

    virtual const char* what() const noexcept;
    /// Returns a static string describing the exception.
    ///
    /// Same as name(), but for compatibility with std::exception.

    const Exception* nested() const;
    /// Returns a pointer to the nested exception, or
    /// null if no nested exception exists.

    const std::string& message() const;
    /// Returns the message text.

    int code() const;
    /// Returns the exception code if defined.

    std::string displayText() const;
    /// Returns a string consisting of the
    /// message name and the message text.

    virtual Exception* clone() const;
    /// Creates an exact copy of the exception.
    ///
    /// The copy can later be thrown again by
    /// invoking rethrow() on it.

    virtual void rethrow() const;
    /// (Re)Throws the exception.
    ///
    /// This is useful for temporarily storing a
    /// copy of an exception (see clone()), then
    /// throwing it again.

protected:
    Exception(int code = 0);
    /// Standard constructor.

    void message(const std::string& msg);
    /// Sets the message for the exception.

    void extendedMessage(const std::string& arg);
    /// Sets the extended message for the exception.

private:
    std::string m_msg;
    Exception* m_pNested;
    int m_code;
};

//
// inlines
//
inline const Exception* Exception::nested() const { return m_pNested; }

inline const std::string& Exception::message() const { return m_msg; }

inline void Exception::message(const std::string& msg) { m_msg = msg; }

inline int Exception::code() const { return m_code; }

//
// Macros for quickly declaring and implementing exception classes.
// Unfortunately, we cannot use a template here because character
// pointers (which we need for specifying the exception name)
// are not allowed as template arguments.
//
#define GLUE_DECLARE_EXCEPTION_CODE(API, CLS, BASE, CODE)                                          \
    class API CLS : public BASE                                                                    \
    {                                                                                              \
    public:                                                                                        \
        CLS(int code = CODE);                                                                      \
        CLS(const std::string& msg, int code = CODE);                                              \
        CLS(const std::string& msg, const std::string& arg, int code = CODE);                      \
        CLS(const std::string& msg, const glue::Exception& exc, int code = CODE);                  \
        CLS(const CLS& exc);                                                                       \
        ~CLS() noexcept;                                                                           \
        CLS& operator=(const CLS& exc);                                                            \
        const char* name() const noexcept;                                                         \
        const char* className() const noexcept;                                                    \
        glue::Exception* clone() const;                                                            \
        void rethrow() const;                                                                      \
    };

#define GLUE_DECLARE_EXCEPTION(API, CLS, BASE) GLUE_DECLARE_EXCEPTION_CODE(API, CLS, BASE, 0)

#define GLUE_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)                                                  \
    CLS::CLS(int code) : BASE(code) {}                                                             \
    CLS::CLS(const std::string& msg, int code) : BASE(msg, code) {}                                \
    CLS::CLS(const std::string& msg, const std::string& arg, int code) : BASE(msg, arg, code) {}   \
    CLS::CLS(const std::string& msg, const glue::Exception& exc, int code)                         \
        : BASE(msg, exc, code) {}                                                                  \
    CLS::CLS(const CLS& exc) : BASE(exc) {}                                                        \
    CLS::~CLS() noexcept {}                                                                        \
    CLS& CLS::operator=(const CLS& exc)                                                            \
    {                                                                                              \
        BASE::operator=(exc);                                                                      \
        return *this;                                                                              \
    }                                                                                              \
    const char* CLS::name() const noexcept { return NAME; }                                        \
    const char* CLS::className() const noexcept { return typeid(*this).name(); }                   \
    glue::Exception* CLS::clone() const { return new CLS(*this); }                                 \
    void CLS::rethrow() const { throw *this; }

//
// Standard exception classes
//
GLUE_DECLARE_EXCEPTION(GLUE_API, LogicException, Exception)
GLUE_DECLARE_EXCEPTION(GLUE_API, AssertionViolationException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, NullPointerException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, NullValueException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, BugcheckException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, InvalidArgumentException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, NotImplementedException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, RangeException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, IllegalStateException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, InvalidAccessException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, SignalException, LogicException)
GLUE_DECLARE_EXCEPTION(GLUE_API, UnhandledException, LogicException)

GLUE_DECLARE_EXCEPTION(GLUE_API, RuntimeException, Exception)
GLUE_DECLARE_EXCEPTION(GLUE_API, NotFoundException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, ExistsException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, TimeoutException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, SystemException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, RegularExpressionException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, LibraryLoadException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, LibraryAlreadyLoadedException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, NoThreadAvailableException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, PropertyNotSupportedException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, PoolOverflowException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, NoPermissionException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, OutOfMemoryException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, DataException, RuntimeException)

GLUE_DECLARE_EXCEPTION(GLUE_API, DataFormatException, DataException)
GLUE_DECLARE_EXCEPTION(GLUE_API, SyntaxException, DataException)
GLUE_DECLARE_EXCEPTION(GLUE_API, CircularReferenceException, DataException)
GLUE_DECLARE_EXCEPTION(GLUE_API, PathSyntaxException, SyntaxException)
GLUE_DECLARE_EXCEPTION(GLUE_API, IOException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, ProtocolException, IOException)
GLUE_DECLARE_EXCEPTION(GLUE_API, FileException, IOException)
GLUE_DECLARE_EXCEPTION(GLUE_API, FileExistsException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, FileNotFoundException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, PathNotFoundException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, FileReadOnlyException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, FileAccessDeniedException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, CreateFileException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, OpenFileException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, WriteFileException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, ReadFileException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, DirectoryNotEmptyException, FileException)
GLUE_DECLARE_EXCEPTION(GLUE_API, UnknownURISchemeException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, TooManyURIRedirectsException, RuntimeException)
GLUE_DECLARE_EXCEPTION(GLUE_API, URISyntaxException, SyntaxException)

GLUE_DECLARE_EXCEPTION(GLUE_API, ApplicationException, Exception)
GLUE_DECLARE_EXCEPTION(GLUE_API, BadCastException, RuntimeException)

GLUE_END_NAMESPACE
