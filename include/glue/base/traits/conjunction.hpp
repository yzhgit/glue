//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <type_traits>

namespace glue {

/// \brief Type trait to determine the bool_constant from a logical
///        AND operation of other bool_constants
///
/// The result is aliased as \c ::value
template <typename...>
struct conjunction;

template <typename B1>
struct conjunction<B1> : B1 {};

template <typename B1, typename... Bn>
struct conjunction<B1, Bn...> : std::conditional_t<B1::value, conjunction<Bn...>, B1> {};

}  // namespace glue
