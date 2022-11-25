//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/traits/type_identity.hpp"

namespace glue {

namespace detail {
/// \brief struct helper for void_t
template <typename...>
struct voidify : type_identity<void> {};
}  // namespace detail

/// \brief Utility metafunction that maps a sequence of any types to the
///        type void
///
/// This is used for determining validity of expressions using SFINAE
template <typename... Types>
using void_t = typename detail::voidify<Types...>::type;

}  // namespace glue
