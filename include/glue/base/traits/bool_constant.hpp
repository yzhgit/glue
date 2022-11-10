//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <type_traits>

namespace glue {

template <typename T, T V>
using integral_constant = std::integral_constant<T, V>;

template <typename T, T V>
using integral_constant_t = typename integral_constant<T, V>::type;

template <typename T, T V>
constexpr T integral_constant_v = integral_constant<T, V>::value;

/// \brief This utility metafunction provides compile-time boolean values
///        as an integral constant
///
/// The value is aliased as \c ::value
template <bool B>
struct bool_constant : integral_constant<bool, B>
{
};

/// \brief Convenience template variable to extract bool_constant::value
///
/// This is always the same as the template variable (trivial)
template <bool B>
constexpr bool bool_constant_v = bool_constant<B>::value;

//-------------------------------------------------------------------------

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

//-------------------------------------------------------------------------

/// \brief Utility metafunction that determines whether \p T is a
///        size_constant
///
/// \tparam T the type to check
/// The result is aliased as \c ::value
template <typename T>
struct is_bool_constant : false_type
{
};

template <bool B>
struct is_bool_constant<bool_constant<B>> : true_type
{
};

} // namespace glue
