//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

#define crypto_secretbox_MACBYTES 16
#define crypto_secretbox_KEYBYTES 32
#define crypto_secretbox_NONCEBYTES 24

namespace glue {

GLUE_API int crypto_secretbox_detached(uint8_t ctext[], uint8_t mac[], const uint8_t ptext[],
                                       size_t ptext_len, const uint8_t nonce[],
                                       const uint8_t key[]);

inline int crypto_secretbox_easy(uint8_t ctext[], const uint8_t ptext[], size_t ptext_len,
                                 const uint8_t nonce[], const uint8_t key[])
{
    return crypto_secretbox_detached(ctext + crypto_secretbox_MACBYTES, ctext, ptext, ptext_len,
                                     nonce, key);
}

GLUE_API int crypto_secretbox_open_detached(uint8_t ptext[], const uint8_t ctext[],
                                            const uint8_t mac[], size_t ctext_len,
                                            const uint8_t nonce[], const uint8_t key[]);

inline int crypto_secretbox_open_easy(uint8_t out[], const uint8_t ctext[], size_t ctext_len,
                                      const uint8_t nonce[], const uint8_t key[])
{
    if (ctext_len < crypto_secretbox_MACBYTES) { return -1; }

    return crypto_secretbox_open_detached(out, ctext + crypto_secretbox_MACBYTES, ctext,
                                          ctext_len - crypto_secretbox_MACBYTES, nonce, key);
}

} // namespace glue
