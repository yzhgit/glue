//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <type_traits>

namespace glue {

/// \brief This utility metafunction provides compile-time boolean values
///        as an integral constant
///
/// The value is aliased as \c ::value
template <bool B>
struct bool_constant : std::integral_constant<bool, B>
{
};

//-------------------------------------------------------------------------

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;


} // namespace glue
