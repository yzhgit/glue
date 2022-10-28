//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "maths/Range.h"
#include "memory/LeakedObjectDetector.h"

namespace glue
{

//==============================================================================
/**
    A random number generator.

    You can create a Random object and use it to generate a sequence of random numbers.

    @tags{Core}
*/
class GLUE_API Random final
{
public:
    //==============================================================================
    /** Creates a Random object based on a seed value.

        For a given seed value, the subsequent numbers generated by this object
        will be predictable, so a good idea is to set this value based
        on the time, e.g.

        new Random (Time::currentTimeMillis())
    */
    explicit Random(int64 seedValue) noexcept;

    /** Creates a Random object using a random seed value.
        Internally, this calls setSeedRandomly() to randomise the seed.
    */
    Random();

    /** Returns the next random 32 bit integer.
        @returns a random integer from the full range 0x80000000 to 0x7fffffff
    */
    int nextInt() noexcept;

    /** Returns the next random number, limited to a given range.
        The maxValue parameter may not be negative, or zero.
        @returns a random integer between 0 (inclusive) and maxValue (exclusive).
    */
    int nextInt(int maxValue) noexcept;

    /** Returns the next random number, limited to a given range.
        @returns a random integer between the range start (inclusive) and its end (exclusive).
    */
    int nextInt(Range<int> range) noexcept;

    /** Returns the next 64-bit random number.
        @returns a random integer from the full range 0x8000000000000000 to 0x7fffffffffffffff
    */
    int64 nextInt64() noexcept;

    /** Returns the next random floating-point number.
        @returns a random value in the range 0 (inclusive) to 1.0 (exclusive)
    */
    float nextFloat() noexcept;

    /** Returns the next random floating-point number.
        @returns a random value in the range 0 (inclusive) to 1.0 (exclusive)
    */
    double nextDouble() noexcept;

    /** Returns the next random boolean value. */
    bool nextBool() noexcept;

    //==============================================================================
    /** Resets this Random object to a given seed value. */
    void setSeed(int64 newSeed) noexcept;

    /** Returns the RNG's current seed. */
    int64 getSeed() const noexcept { return seed; }

    /** Merges this object's seed with another value.
        This sets the seed to be a value created by combining the current seed and this
        new value.
    */
    void combineSeed(int64 seedValue) noexcept;

    /** Reseeds this generator using a value generated from various semi-random system
        properties like the current time, etc.

        Because this function convolves the time with the last seed value, calling
        it repeatedly will increase the randomness of the final result.
    */
    void setSeedRandomly();

    /** The overhead of creating a new Random object is fairly small, but if you want to avoid
        it, you can call this method to get a global shared Random object.

        It's not thread-safe though, so threads should use their own Random object, otherwise
        you run the risk of your random numbers becoming.. erm.. randomly corrupted..
    */
    static Random& getSystemRandom() noexcept;

private:
    //==============================================================================
    int64 seed;

    GLUE_LEAK_DETECTOR(Random)
};

} // namespace glue
