//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/traits/type_identity.hpp"

namespace glue {
namespace detail {

template <typename T>
struct is_reference_wrapper : std::false_type {};

template <typename U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};

template <typename T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

template <typename Base, typename T, typename Derived, typename... Args>
inline constexpr auto invoke_impl(T Base::*pmf, Derived&& ref, Args&&... args) noexcept(
    noexcept((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...)))
    -> std::enable_if_t<std::is_function<T>::value &&
                            std::is_base_of<Base, std::decay_t<Derived>>::value,
                        decltype((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...))> {
    return (std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...);
}

template <typename Base, typename T, typename RefWrap, typename... Args>
inline constexpr auto invoke_impl(T Base::*pmf, RefWrap&& ref, Args&&... args) noexcept(
    noexcept((ref.get().*pmf)(std::forward<Args>(args)...)))
    -> std::enable_if_t<std::is_function<T>::value &&
                            is_reference_wrapper<std::decay_t<RefWrap>>::value,
                        decltype((ref.get().*pmf)(std::forward<Args>(args)...))>

{
    return (ref.get().*pmf)(std::forward<Args>(args)...);
}

template <typename Base, typename T, typename Pointer, typename... Args>
inline constexpr auto invoke_impl(T Base::*pmf, Pointer&& ptr, Args&&... args) noexcept(
    noexcept(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...)))
    -> std::enable_if_t<
        std::is_function<T>::value && !is_reference_wrapper<std::decay_t<Pointer>>::value &&
            !std::is_base_of<Base, std::decay_t<Pointer>>::value,
        decltype(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...))> {
    return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
}

template <typename Base, typename T, typename Derived>
inline constexpr auto invoke_impl(T Base::*pmd,
                                  Derived&& ref) noexcept(noexcept(std::forward<Derived>(ref).*pmd))
    -> std::enable_if_t<!std::is_function<T>::value &&
                            std::is_base_of<Base, std::decay_t<Derived>>::value,
                        decltype(std::forward<Derived>(ref).*pmd)> {
    return std::forward<Derived>(ref).*pmd;
}

template <typename Base, typename T, typename RefWrap>
inline constexpr auto invoke_impl(T Base::*pmd, RefWrap&& ref) noexcept(noexcept(ref.get().*pmd))
    -> std::enable_if_t<!std::is_function<T>::value &&
                            is_reference_wrapper<std::decay_t<RefWrap>>::value,
                        decltype(ref.get().*pmd)> {
    return ref.get().*pmd;
}

template <typename Base, typename T, typename Pointer>
inline constexpr auto invoke_impl(T Base::*pmd,
                                  Pointer&& ptr) noexcept(noexcept((*std::forward<Pointer>(ptr)).*
                                                                   pmd))
    -> std::enable_if_t<!std::is_function<T>::value &&
                            !is_reference_wrapper<std::decay_t<Pointer>>::value &&
                            !std::is_base_of<Base, std::decay_t<Pointer>>::value,
                        decltype((*std::forward<Pointer>(ptr)).*pmd)> {
    return (*std::forward<Pointer>(ptr)).*pmd;
}

template <typename F, typename... Args>
inline constexpr auto invoke_impl(F&& f, Args&&... args) noexcept(
    noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
    -> std::enable_if_t<!std::is_member_pointer<std::decay_t<F>>::value,
                        decltype(std::forward<F>(f)(std::forward<Args>(args)...))> {
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

//=======================================================================
// is_invocable
//=======================================================================

template <typename Fn, typename... Args>
struct is_invocable {
    template <typename Fn2, typename... Args2>
    static auto test(Fn2&&, Args2&&...)
        -> decltype(invoke_impl(std::declval<Fn2>(), std::declval<Args2>()...), std::true_type{});

    static auto test(...) -> std::false_type;

    static constexpr bool value =
        decltype(test(std::declval<Fn>(), std::declval<Args>()...))::value;
};

//=======================================================================
// is_nothrow_invocable
//=======================================================================

template <typename Fn, typename... Args>
struct is_nothrow_invocable {
    template <typename Fn2, typename... Args2>
    static auto test(Fn2&&, Args2&&...)
        -> decltype(invoke_impl(std::declval<Fn2>(), std::declval<Args2>()...),
                    std::integral_constant<bool,
                                           noexcept(invoke_impl(std::declval<Fn2>(),
                                                                std::declval<Args2>()...))>{});

    static auto test(...) -> std::false_type;

    static constexpr bool value =
        decltype(test(std::declval<Fn>(), std::declval<Args>()...))::value;
};

}  // namespace detail

/// \brief Invoke the Callable object \p function with the parameters \p args.
///
/// As by \c INVOKE(std::forward<F>(f), std::forward<Args>(args)...)
///
/// \param function Callable object to be invoked
/// \param args     arguments to pass to \p function
template <typename Func, typename... Args>
constexpr auto invoke(Func&& function, Args&&... args)
#ifndef _MSC_VER
    noexcept(noexcept(detail::invoke_impl(std::forward<Func>(function),
                                          std::forward<Args>(args)...)))
#endif
        -> decltype(detail::invoke_impl(std::forward<Func>(function), std::forward<Args>(args)...));

template <typename F, typename... Types>
struct invoke_result
    : type_identity<decltype(invoke(std::declval<F>(), std::declval<Types>()...))> {};

template <typename F, typename... Types>
using invoke_result_t = typename invoke_result<F, Types...>::type;

/// \brief Type trait to determine whether \p Fn is invokable with
///        \p Args...
///
/// Formally, the expression:
/// \code
/// INVOKE( std::declval<Fn>(), std::declval<Args>()... )
/// \endcode
/// is well formed
///
/// The result is aliased as \c ::value
template <typename Fn, typename... Args>
using is_invocable = detail::is_invocable<Fn, Args...>;

/// \brief Type-trait helper to retrieve the \c ::value of is_invokable
template <typename Fn, typename... Args>
constexpr bool is_invocable_v = is_invocable<Fn, Args...>::value;

//    template<typename R, typename Fn, typename... Types>
//    using is_invocable_r =
//    is_detected_convertible<R,invoke_result_t,Fn,Types...>;
//
//    template<typename R, typename Fn, typename... Types>
//    constexpr bool is_invocable_r_v = is_invocable_r<R,Fn,Types...>::value;

//------------------------------------------------------------------------

/// \brief Type trait to determine whether \p Fn is nothrow invokable with
///        \p Args...
///
/// Formally, the expression:
/// \code
/// INVOKE( std::declval<Fn>(), std::declval<Args>()... )
/// \endcode
/// is well formed and is known not to throw
///
/// The result is aliased as \c ::value
template <typename Fn, typename... Args>
using is_nothrow_invocable = detail::is_nothrow_invocable<Fn, Args...>;

/// \brief Type-trait helper to retrieve the \c ::value of is_nothrow_invokable
template <typename Fn, typename... Args>
constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Fn, Args...>::value;

}  // namespace glue

#include "glue/base/detail/invoke.inl"
