//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

namespace glue {

GLUE_API void ed25519_gen_keypair(uint8_t pk[32], uint8_t sk[64], const uint8_t seed[32]);

GLUE_API void ed25519_sign(uint8_t sig[64],
                           const uint8_t msg[],
                           size_t msg_len,
                           const uint8_t sk[64],
                           const uint8_t domain_sep[],
                           size_t domain_sep_len);

GLUE_API bool ed25519_verify(const uint8_t msg[],
                             size_t msg_len,
                             const uint8_t sig[64],
                             const uint8_t pk[32],
                             const uint8_t domain_sep[],
                             size_t domain_sep_len);

}  // namespace glue
