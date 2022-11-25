//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

namespace glue {

/// \brief Utility metafunction that always expands into an empty struct
///        with no members or type members
template <typename T = void>
struct empty {};

}  // namespace glue
