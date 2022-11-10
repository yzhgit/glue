//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/traits/bool_constant.hpp"

namespace glue {

/// \brief Type trait to determine the \c bool_constant from a logical
///        OR operations of other bool_constant
///
/// The result is aliased as \c ::value
template <typename...>
struct disjunction : false_type
{
};

template <typename B1>
struct disjunction<B1> : B1
{
};

template <typename B1, typename... Bn>
struct disjunction<B1, Bn...> : std::conditional_t<B1::value != false, B1, disjunction<Bn...>>
{
};

} // namespace glue
