//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/standard_header.h"


// Assertions default to enabled in debug mode unless otherwise specified
#ifndef BIT_CORE_ASSERTIONS_ENABLED
# ifdef BIT_DEBUG
#   define BIT_CORE_ASSERTIONS_ENABLED 1
# endif
#endif

#ifdef BIT_ALWAYS_ASSERT
# error duplicate definition of BIT_ALWAYS_ASSERT
#endif

#ifdef BIT_ASSERT
# error duplicate definition of BIT_ASSERT
#endif

#ifdef BIT_INTERNAL_ASSERT_1
# error duplicate definition of BIT_INTERNAL_ASSERT_1
#endif

#ifdef BIT_INTERNAL_ASSERT_2
# error duplicate definition of BIT_INTERNAL_ASSERT_2
#endif

#define BIT_INTERNAL_ASSERT_1(condition) \
  ((BIT_LIKELY(condition)) \
    ? ((void)0) \
    : []( ::bit::core::source_location source ) \
      { \
        ::bit::core::detail::assert_internal( \
          "assertion failure: condition '" \
          BIT_STRINGIZE(condition) \
          "' failed.", \
          source \
        ); \
      }( BIT_MAKE_SOURCE_LOCATION() ) )

#define BIT_INTERNAL_ASSERT_2(condition,message) \
  ((BIT_LIKELY(condition)) \
    ? ((void)0) \
    : []( ::bit::core::source_location source ) \
      { \
        ::bit::core::detail::assert_internal( \
          "assertion failure: condition '" \
          BIT_STRINGIZE(condition) \
          "' failed with message \"" \
          message "\"", \
          source \
        ); \
      }( BIT_MAKE_SOURCE_LOCATION() ) )

//! \def BIT_ASSERT(condition, message)
//!
//! \brief A runtime assertion when \a condition fails, displaying \a message
//!        to the user.
//!
//! An assertion will report the error, the source location of the error,
//! and trigger a breakpoint (if the debugger is open). After the breakpoint
//! trigger, it invokes std::terminate.
//!
//! \note This assertion is always enabled, regardless of the state of
//!       BIT_CORE_ASSERTIONS_ENABLED
//!
//! \param condition the condition that, when false, triggers an assertion
//! \param message   the message for the failure [optional]
#define BIT_ALWAYS_ASSERT(...) \
  BIT_JOIN( BIT_INTERNAL_ASSERT_, BIT_COUNT_VA_ARGS(__VA_ARGS__) )(__VA_ARGS__)

//! \def BIT_ASSERT(condition, message, ...)
//!
//! \brief A runtime assertion when \a condition fails, displaying \a message
//!        to the user.
//!
//! This simply calls 'BIT_ALWAYS_ASSERT' when assertions are enabled
//!
//! \param condition the condition that, when false, triggers an assertion
//! \param message   the message for the failure [optional]
#if BIT_CORE_ASSERTIONS_ENABLED
# define BIT_ASSERT(...) BIT_ALWAYS_ASSERT(__VA_ARGS__)
#else
# define BIT_ASSERT(...) ((void)0)
#endif

#ifdef BIT_DEBUG
# define BIT_ASSERT_AND_ASSUME(cond) BIT_ASSERT(cond)
#else
# define BIT_ASSERT_AND_ASSUME(cond) BIT_ASSUME(cond)
#endif

//! \def BIT_ASSERT_OR_THROW(condition, exception, message)
//!
//! \brief An assertion that either throws the given exception, if exceptions
//!        are enabled, or calls the default assert
//!
//! \param condition the condition that, when false, triggers either an
//!                  assertion or throws an exception
//! \param exception the exception to throw
//! \param message   the message for the failure
#if BIT_COMPILER_EXCEPTIONS_ENABLED
# define BIT_ASSERT_OR_THROW(condition,exception,message) \
  ((BIT_LIKELY(condition)) ? ((void)0) : []{ throw exception{ message }; }())
#else
# define BIT_ASSERT_OR_THROW(condition,exception,message) \
  BIT_ASSERT(condition,message)
#endif

#include "detail/assert.inl"

