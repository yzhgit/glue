//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <type_traits>

namespace glue {

/// \brief Utility metafunction for negating a bool_constant
///
/// The result is aliased as \c ::value
///
/// \tparam B the constant
template <typename B>
struct negation : std::bool_constant<!bool(B::value)> {};

}  // namespace glue
