//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

namespace glue {

/// \brief This simple type is used to add '::type' to any transformation
///        type-traits
///
/// The result is aliased as \c ::type
template <typename T>
struct type_identity
{
    using type = T;
};

/// \brief Namespace alias used to extract identity<T>::type
///
/// \note This alias is mostly useful for preventing template deduction
template <typename T>
using type_identity_t = typename type_identity<T>::type;

} // namespace glue
