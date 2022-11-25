//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/crypto/sodium.h"

#include "glue/crypto/poly1305.h"
#include "glue/crypto/salsa20.h"
#include "mem_ops.h"

namespace glue {

GLUE_API int crypto_secretbox_detached(uint8_t ctext[],
                                       uint8_t mac[],
                                       const uint8_t ptext[],
                                       size_t ptext_len,
                                       const uint8_t nonce[],
                                       const uint8_t key[]) {
    Salsa20 salsa;
    salsa.set_key(key, crypto_secretbox_KEYBYTES);
    salsa.set_iv(nonce, crypto_secretbox_NONCEBYTES);

    std::vector<uint8_t> auth_key(32);
    salsa.write_keystream(auth_key.data(), auth_key.size());

    salsa.cipher(ptext, ctext, ptext_len);

    Poly1305 poly1305;
    poly1305.set_key(auth_key.data(), auth_key.size());
    poly1305.update(ctext, ptext_len);
    poly1305.final(mac);

    return 0;
}

GLUE_API int crypto_secretbox_open_detached(uint8_t ptext[],
                                            const uint8_t ctext[],
                                            const uint8_t mac[],
                                            size_t ctext_len,
                                            const uint8_t nonce[],
                                            const uint8_t key[]) {
    Salsa20 salsa;
    salsa.set_key(key, crypto_secretbox_KEYBYTES);
    salsa.set_iv(nonce, crypto_secretbox_NONCEBYTES);

    std::vector<uint8_t> auth_key(32);
    salsa.write_keystream(auth_key.data(), auth_key.size());

    Poly1305 poly1305;
    poly1305.set_key(auth_key.data(), auth_key.size());
    poly1305.update(ctext, ctext_len);

    std::vector<uint8_t> computed_mac(poly1305.output_length());
    poly1305.final(computed_mac.data());

    if (!constant_time_compare(mac, computed_mac.data(), computed_mac.size())) return -1;

    salsa.cipher(ctext, ptext, ctext_len);

    return 0;
}

}  // namespace glue
