//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/hash.hpp" // hash_t

namespace glue {

namespace detail {

    struct tribool_t
    {
        constexpr tribool_t() = delete;
        explicit constexpr tribool_t(int){};
    };

} // namespace detail

//=========================================================================
// class : tribool
//=========================================================================

class tribool;

/// The type used to mark an indeterminate tribool
using indeterminate_t = bool (*)(const tribool&, detail::tribool_t);

///////////////////////////////////////////////////////////////////////////
/// \brief This class is a 3-state boolean type that introduces
///        'indeterminate' as the third state.
///
/// Normal boolean logic is still supported
///////////////////////////////////////////////////////////////////////////
class tribool
{
    //----------------------------------------------------------------------
    // Constructor / Assignment
    //----------------------------------------------------------------------
public:
    /// \brief Constructs this tribool with an indeterminate state
    constexpr tribool();

    /// \brief Constructs this tribool with a boolean state
    ///
    /// \param b the boolean state to initialize this tribool
    constexpr tribool(bool b);

    /// \brief Constructs this tribool with an indeterminate state
    constexpr tribool(indeterminate_t);

    /// \brief Copy-constructs this tribool
    ///
    /// \param other the other tribool to copy
    constexpr tribool(const tribool& other) = default;

    /// \brief Move-constructs a tribool
    ///
    /// \param other the other tribool to move
    constexpr tribool(tribool&& other) = default;

    //----------------------------------------------------------------------

    /// \brief Copy-assigns a tribool
    ///
    /// \param other the other tribool to copy
    tribool& operator=(const tribool& other) = default;

    /// \brief Move-assigns a tribool
    ///
    /// \param other the other tribool to move
    tribool& operator=(tribool&& other) = default;

    //----------------------------------------------------------------------
    // Observers
    //----------------------------------------------------------------------
public:
    /// \brief Converts this tribool to a boolean value
    ///
    /// Indeterminate states translate to false
    constexpr explicit operator bool() const noexcept;

    /// \brief Negates this tribool to a boolean value
    ///
    /// Indeterminate states do not negate
    ///
    /// \return a tribool containing the negated state
    constexpr tribool operator!() const noexcept;

    //----------------------------------------------------------------------
    // Private Member Types
    //----------------------------------------------------------------------
private:
    enum class state : unsigned char
    {
        e_false = 0,
        e_true = 1,
        e_indeterminate = 2
    };

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
private:
    state m_state;

    //----------------------------------------------------------------------
    // Friends
    //----------------------------------------------------------------------
private:
    friend constexpr bool operator==(const tribool&, const tribool&) noexcept;
    friend constexpr tribool operator&&(const tribool&, const tribool&) noexcept;
    friend constexpr tribool operator||(const tribool&, const tribool&) noexcept;
    friend constexpr bool indeterminate(const tribool&, detail::tribool_t) noexcept;
};

//=========================================================================
// non-member functions : class : tribool
//=========================================================================

//------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------

/// \brief Determines whether the given tribool is indeterminate
///
/// \param t the tribool
/// \return \c true if \p t is \p indeterminate
#ifndef DOXYGEN
constexpr bool indeterminate(const tribool& t, detail::tribool_t = detail::tribool_t{0}) noexcept;
#else
constexpr bool indeterminate(const tribool& t) noexcept;
#endif

constexpr hash_t hash_value(tribool t) noexcept;

//------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------

/// \brief Equality comparison between two tribooleans
///
/// \param lhs the left entry
/// \param rhs the right entry
/// \return \c true if \p lhs has the same state as \p rhs
constexpr bool operator==(const tribool& lhs, const tribool& rhs) noexcept;
constexpr bool operator==(indeterminate_t, const tribool& rhs) noexcept;
constexpr bool operator==(const tribool& lhs, indeterminate_t) noexcept;
constexpr bool operator==(const tribool& lhs, bool rhs) noexcept;
constexpr bool operator==(bool lhs, const tribool& rhs) noexcept;

/// \brief Equality comparison between two tribooleans
///
/// \param lhs the left entry
/// \param rhs the right entry
/// \return \c true if \p lhs has the same state as \p rhs
constexpr bool operator!=(const tribool& lhs, const tribool& rhs) noexcept;
constexpr bool operator!=(indeterminate_t, const tribool& rhs) noexcept;
constexpr bool operator!=(const tribool& lhs, indeterminate_t) noexcept;
constexpr bool operator!=(const tribool& lhs, bool rhs) noexcept;
constexpr bool operator!=(bool lhs, const tribool& rhs) noexcept;

constexpr tribool operator&&(const tribool& lhs, const tribool& rhs) noexcept;
constexpr tribool operator||(const tribool& lhs, const tribool& rhs) noexcept;

} // namespace glue

#include "detail/tribool.inl"
