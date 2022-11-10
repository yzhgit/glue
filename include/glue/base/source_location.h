//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

namespace glue {

///////////////////////////////////////////////////////////////////////////
/// \brief Source location is a small, immutable type used for representing
///        where retrieving information about where this type is constructed
///
/// The common approach is intended for this class to be embedded within
/// exceptions and assertions in order to easily runtime-query the exact
/// location an exception was thrown.
///////////////////////////////////////////////////////////////////////////
class source_location
{
public:
    /// \brief Constructs a source_location with the given \p file_name,
    ///        \p function_name, and \p line
    ///
    /// \param file_name     The name of the file
    /// \param function_name The name of the function
    /// \param line          The line number of the given
    constexpr source_location(const char* file_name, const char* function_name, std::size_t line);

    /// \brief Copies a given source_location
    constexpr source_location(const source_location&) = default;

    /// \brief Moves a given source_location
    constexpr source_location(source_location&&) = default;

public:
    /// \brief Retrieves the line o the source_location
    ///
    /// \return the line of the source file
    constexpr std::size_t line() const noexcept;

    /// \brief Retrieves the function_name of the source_location
    ///
    /// \return the name of the function
    constexpr const char* function_name() const noexcept;

    /// \brief Retrieves the file_name of the source_location
    ///
    /// \return the name of the file
    constexpr const char* file_name() const noexcept;

    //-----------------------------------------------------------------------
    // Private Members
    //-----------------------------------------------------------------------
private:
    const char* m_file;     ///< The current file
    const char* m_function; ///< The current function
    std::size_t m_line;     ///< The line number
};

/// \brief Determines equality between two source_locations
///
/// \param lhs The source_location on the left side of the equation
/// \param rhs The source_location on the right side of the equation
/// \return \c true if \p lhs is equal to \p rhs
inline bool operator==(const source_location& lhs, const source_location& rhs) noexcept
{
    return lhs.file_name() == rhs.file_name() && lhs.function_name() == rhs.function_name() &&
           lhs.line() == rhs.line();
}

/// \brief Determines inequality between two source_locations
///
/// \param lhs The source_location on the left side of the equation
/// \param rhs The source_location on the right side of the equation
/// \return \c true if \p lhs is not equal to \p rhs
inline bool operator!=(const source_location& lhs, const source_location& rhs) noexcept
{
    return !(lhs == rhs);
}

inline constexpr glue::source_location::source_location(const char* file_name,
                                                        const char* function_name, std::size_t line)
    : m_file{file_name}, m_function{function_name}, m_line{line}
{}

inline constexpr std::size_t glue::source_location::line() const noexcept
{
    return m_line;
}

inline constexpr const char* glue::source_location::function_name() const noexcept
{
    return m_function;
}

inline constexpr const char* glue::source_location::file_name() const noexcept
{
    return m_file;
}

//! \def GLUE_MAKE_SOURCE_LOCATION
//!
//! \brief Retrieves the current source location for where this macro is
//!        invoked
#define GLUE_MAKE_SOURCE_LOCATION() ::glue::source_location(__FILE__, __func__, __LINE__)

} // namespace glue
