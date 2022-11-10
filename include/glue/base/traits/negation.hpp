//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/traits/bool_constant.hpp"

namespace glue {

/// \brief Utility metafunction for negating a bool_constant
///
/// The result is aliased as \c ::value
///
/// \tparam B the constant
template <typename B>
struct negation : bool_constant<!bool(B::value)>
{
};

} // namespace glue
