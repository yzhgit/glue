//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

#include <exception>
#include <type_traits>
#include <utility>

namespace glue {

///////////////////////////////////////////////////////////////////////////
/// \brief This class leverages RAII to perform cleanup actions at the end
///        of a scope
///
/// This is guaranteed to run the action, even if an exception is thrown
///////////////////////////////////////////////////////////////////////////
template <typename Fn>
class ScopeGuard
{
public:
    enum class scope : char
    {
        cancelled = 0x0,     ///< Don't execute
        on_clean_exit = 0x1, ///< Execute on clean
        on_error_exit = 0x2, ///< Execute on error
        on_exit = 0x4,       ///< Execute on either clean or error
    };

public:
    /// \brief Constructs an action to perform on end-of-scope
    ///
    /// \param scope the time to execute the ScopeGuard
    /// \param function The function to call at the end
    constexpr ScopeGuard(scope scope,
                         Fn function) noexcept(std::is_nothrow_move_constructible<Fn>::value);

    /// \brief Move-constructs a final act to perform on end-of-scope
    ///
    /// \param other the other act to move
    constexpr ScopeGuard(ScopeGuard&& other) noexcept(
        std::is_nothrow_move_constructible<Fn>::value);

    // Disallow copy-construction
    ScopeGuard(const ScopeGuard&) = delete;

    /// \brief Destroys the final act, invoking the action
    ~ScopeGuard();

    /// \brief Move-assigns a final act to perform an end-of-scope
    ///
    /// \param other the other act to move
    ScopeGuard& operator=(ScopeGuard&& other) noexcept(std::is_nothrow_move_assignable<Fn>::value);

    // Disallow copy-assignment
    ScopeGuard& operator=(const ScopeGuard&) = delete;

public:
    /// \brief Cancels the final act from occurring
    void cancel() noexcept;

private:
    Fn m_action;   ///< The action to perform
    scope m_scope; ///< When to invoke the scope guard
};

template <typename Fn>
inline constexpr bit::core::ScopeGuard<Fn>::ScopeGuard(scope scope, Fn function) noexcept(
    std::is_nothrow_move_constructible<Fn>::value)
    : m_action(std::move(function)), m_scope(scope)
{}

template <typename Fn>
inline constexpr bit::core::ScopeGuard<Fn>::ScopeGuard(ScopeGuard&& other) noexcept(
    std::is_nothrow_move_constructible<Fn>::value)
    : m_action(std::move(other.m_action)), m_scope(other.m_scope)
{
    other.m_scope = scope::cancelled;
}

template <typename Fn>
inline bit::core::ScopeGuard<Fn>::~ScopeGuard()
{
    if (m_scope == scope::cancelled) return;
#if __cplusplus >= 201703L
    auto has_exceptions = (std::uncaught_exceptions() > 0);
#else
    auto has_exceptions = std::uncaught_exception();
#endif
    if ((m_scope == scope::on_exit) || ((m_scope == scope::on_clean_exit) && !has_exceptions) ||
        ((m_scope == scope::on_error_exit) && has_exceptions))
    {
        m_action();
    }
}

template <typename Fn>
inline bit::core::ScopeGuard<Fn>& bit::core::ScopeGuard<Fn>::operator=(ScopeGuard&& other) noexcept(
    std::is_nothrow_move_assignable<Fn>::value)
{
    m_action = std::move(other.m_action);
    m_scope = other.m_scope;

    other.m_scope = scope::cancelled;
}

template <typename Fn>
inline void bit::core::ScopeGuard<Fn>::cancel() noexcept
{
    m_scope = scope::cancelled;
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

/// \brief Creates a scope guard that executes at the end of the scope
///
/// \param fn the function to execute when a scope is exited
template <typename Fn>
inline constexpr ScopeGuard<std::decay_t<Fn>> on_scope_exit(Fn&& fn)
{
    using scope = typename ScopeGuard<std::decay_t<Fn>>::scope;
    return {scope::on_exit, std::forward<Fn>(fn)};
}

/// \brief Creates a scope guard that executes at the end of the scope only
///        if an exception has been thrown
///
/// \param fn the function to execute when a scope is exited by error
template <typename Fn>
inline constexpr ScopeGuard<std::decay_t<Fn>> on_scope_error_exit(Fn&& fn)
{
    using scope = typename ScopeGuard<std::decay_t<Fn>>::scope;
    return {scope::on_error_exit, std::forward<Fn>(fn)};
}

/// \brief Creates a scope guard that executes at the end of the scope only
///        if no exception has propagated
///
/// \param fn the function to execute when a scope is without error
template <typename Fn>
inline constexpr ScopeGuard<std::decay_t<Fn>> on_scope_clean_exit(Fn&& fn)
{
    using scope = typename ScopeGuard<std::decay_t<Fn>>::scope;
    return {scope::on_clean_exit, std::forward<Fn>(fn)};
}

} // namespace glue
