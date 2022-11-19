//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "bit_ops.h"
#include "secmem.h"

#include <type_traits>
#include <vector>

namespace glue {

namespace CT {

    /**
     * A Mask type used for constant-time operations. A Mask<T> always has value
     * either 0 (all bits cleared) or ~0 (all bits set). All operations in a Mask<T>
     * are intended to compile to code which does not contain conditional jumps.
     * This must be verified with tooling (eg binary disassembly or using valgrind)
     * since you never know what a compiler might do.
     */
    template <typename T>
    class Mask
    {
    public:
        static_assert(std::is_unsigned<T>::value,
                      "CT::Mask only defined for unsigned integer types");

        Mask(const Mask<T>& other) = default;
        Mask<T>& operator=(const Mask<T>& other) = default;

        /**
         * Derive a Mask from a Mask of a larger type
         */
        template <typename U>
        Mask(Mask<U> o) : m_mask(static_cast<T>(o.value()))
        {
            static_assert(sizeof(U) > sizeof(T), "sizes ok");
        }

        /**
         * Return a Mask<T> with all bits set
         */
        static Mask<T> set()
        {
            return Mask<T>(static_cast<T>(~0));
        }

        /**
         * Return a Mask<T> with all bits cleared
         */
        static Mask<T> cleared()
        {
            return Mask<T>(0);
        }

        /**
         * Return a Mask<T> which is set if v is != 0
         */
        static Mask<T> expand(T v)
        {
            return ~Mask<T>::is_zero(v);
        }

        /**
         * Return a Mask<T> which is set if m is set
         */
        template <typename U>
        static Mask<T> expand(Mask<U> m)
        {
            static_assert(sizeof(U) < sizeof(T), "sizes ok");
            return ~Mask<T>::is_zero(m.value());
        }

        /**
         * Return a Mask<T> which is set if v is == 0 or cleared otherwise
         */
        static Mask<T> is_zero(T x)
        {
            return Mask<T>(ct_is_zero<T>(x));
        }

        /**
         * Return a Mask<T> which is set if x == y
         */
        static Mask<T> is_equal(T x, T y)
        {
            return Mask<T>::is_zero(static_cast<T>(x ^ y));
        }

        /**
         * Return a Mask<T> which is set if x < y
         */
        static Mask<T> is_lt(T x, T y)
        {
            return Mask<T>(expand_top_bit<T>(x ^ ((x ^ y) | ((x - y) ^ x))));
        }

        /**
         * Return a Mask<T> which is set if x > y
         */
        static Mask<T> is_gt(T x, T y)
        {
            return Mask<T>::is_lt(y, x);
        }

        /**
         * Return a Mask<T> which is set if x <= y
         */
        static Mask<T> is_lte(T x, T y)
        {
            return ~Mask<T>::is_gt(x, y);
        }

        /**
         * Return a Mask<T> which is set if x >= y
         */
        static Mask<T> is_gte(T x, T y)
        {
            return ~Mask<T>::is_lt(x, y);
        }

        static Mask<T> is_within_range(T v, T l, T u)
        {
            const T v_lt_l = v ^ ((v ^ l) | ((v - l) ^ v));
            const T v_gt_u = u ^ ((u ^ v) | ((u - v) ^ u));
            const T either = v_lt_l | v_gt_u;
            return ~Mask<T>(expand_top_bit(either));
        }

        static Mask<T> is_any_of(T v, std::initializer_list<T> accepted)
        {
            T accept = 0;

            for (auto a : accepted)
            {
                const T diff = a ^ v;
                const T eq_zero = ~diff & (diff - 1);
                accept |= eq_zero;
            }

            return Mask<T>(expand_top_bit(accept));
        }

        /**
         * AND-combine two masks
         */
        Mask<T>& operator&=(Mask<T> o)
        {
            m_mask &= o.value();
            return (*this);
        }

        /**
         * XOR-combine two masks
         */
        Mask<T>& operator^=(Mask<T> o)
        {
            m_mask ^= o.value();
            return (*this);
        }

        /**
         * OR-combine two masks
         */
        Mask<T>& operator|=(Mask<T> o)
        {
            m_mask |= o.value();
            return (*this);
        }

        /**
         * AND-combine two masks
         */
        friend Mask<T> operator&(Mask<T> x, Mask<T> y)
        {
            return Mask<T>(x.value() & y.value());
        }

        /**
         * XOR-combine two masks
         */
        friend Mask<T> operator^(Mask<T> x, Mask<T> y)
        {
            return Mask<T>(x.value() ^ y.value());
        }

        /**
         * OR-combine two masks
         */
        friend Mask<T> operator|(Mask<T> x, Mask<T> y)
        {
            return Mask<T>(x.value() | y.value());
        }

        /**
         * Negate this mask
         */
        Mask<T> operator~() const
        {
            return Mask<T>(~value());
        }

        /**
         * Return x if the mask is set, or otherwise zero
         */
        T if_set_return(T x) const
        {
            return m_mask & x;
        }

        /**
         * Return x if the mask is cleared, or otherwise zero
         */
        T if_not_set_return(T x) const
        {
            return ~m_mask & x;
        }

        /**
         * If this mask is set, return x, otherwise return y
         */
        T select(T x, T y) const
        {
            return static_cast<T>(y ^ (value() & (x ^ y)));
        }

        T select_and_unpoison(T x, T y) const
        {
            T r = this->select(x, y);
            return r;
        }

        /**
         * If this mask is set, return x, otherwise return y
         */
        Mask<T> select_mask(Mask<T> x, Mask<T> y) const
        {
            return Mask<T>(select(x.value(), y.value()));
        }

        /**
         * Conditionally set output to x or y, depending on if mask is set or
         * cleared (resp)
         */
        void select_n(T output[], const T x[], const T y[], size_t len) const
        {
            for (size_t i = 0; i != len; ++i) output[i] = this->select(x[i], y[i]);
        }

        /**
         * If this mask is set, zero out buf, otherwise do nothing
         */
        void if_set_zero_out(T buf[], size_t elems)
        {
            for (size_t i = 0; i != elems; ++i) { buf[i] = this->if_not_set_return(buf[i]); }
        }

        /**
         * Return the value of the mask, unpoisoned
         */
        T unpoisoned_value() const
        {
            T r = value();
            return r;
        }

        /**
         * Return true iff this mask is set
         */
        bool is_set() const
        {
            return unpoisoned_value() != 0;
        }

        /**
         * Return the underlying value of the mask
         */
        T value() const
        {
            return m_mask;
        }

    private:
        Mask(T m) : m_mask(m)
        {}

        T m_mask;
    };

} // namespace CT

} // namespace glue
