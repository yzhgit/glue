//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

namespace glue {

//==============================================================================
/**
    Holds an absolute date and time.

    Internally, the time is stored at millisecond precision.

    @see RelativeTime

    @tags{Core}
*/
class GLUE_API Time {
   public:
    /** Returns the current system time.

        Returns the number of milliseconds since midnight Jan 1st 1970 UTC.

        Should be accurate to within a few millisecs, depending on platform,
        hardware, etc.
    */
    static int64 currentTimeMillis() noexcept;

    /** Returns the number of millisecs since a fixed event (usually system
       startup).

        This has the same function as getMillisecondCounter(), but returns a
       more accurate value, using a higher-resolution timer if one is available.

        @see getMillisecondCounter
    */
    static double getMillisecondCounterHiRes() noexcept;

    /** Returns the current high-resolution counter's tick-count.

        This is a similar idea to getMillisecondCounter(), but with a higher
        resolution.

        @see getHighResolutionTicksPerSecond, highResolutionTicksToSeconds,
             secondsToHighResolutionTicks
    */
    static int64 getHighResolutionTicks() noexcept;

    /** Returns the resolution of the high-resolution counter in ticks per
       second.

        @see getHighResolutionTicks, highResolutionTicksToSeconds,
             secondsToHighResolutionTicks
    */
    static int64 getHighResolutionTicksPerSecond() noexcept;

    /** Converts a number of high-resolution ticks into seconds.

        @see getHighResolutionTicks, getHighResolutionTicksPerSecond,
             secondsToHighResolutionTicks
    */
    static double highResolutionTicksToSeconds(int64 ticks) noexcept;

    /** Converts a number seconds into high-resolution ticks.

        @see getHighResolutionTicks, getHighResolutionTicksPerSecond,
             highResolutionTicksToSeconds
    */
    static int64 secondsToHighResolutionTicks(double seconds) noexcept;
};

}  // namespace glue
