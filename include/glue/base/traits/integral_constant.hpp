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

} // namespace glue
