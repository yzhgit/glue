//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/StringArray.h"

namespace glue
{

//==============================================================================
/**
    Contains methods for finding out about the current hardware and OS configuration.

    @tags{Core}
*/
class GLUE_API SystemStats final
{
public:
    //==============================================================================
    /** Returns the current version of GLUE,
        See also the JUCE_VERSION, JUCE_MAJOR_VERSION and JUCE_MINOR_VERSION macros.
    */
    static String getVersion();

    //==============================================================================
    /** The set of possible results of the getOperatingSystemType() method. */
    enum OperatingSystemType
    {
        UnknownOS = 0,
        /**
         To test whether any version of Windows is running, you can use the expression
         ((getOperatingSystemType() & Windows) !=0).
        */
        Windows = 0x0200,
        Linux = 0x0400,
        Android = 0x0800,

        Win2000 = Windows | 1,
        WinXP = Windows | 2,
        WinVista = Windows | 3,
        Windows7 = Windows | 4,
        Windows8_0 = Windows | 5,
        Windows8_1 = Windows | 6,
        Windows10 = Windows | 7,
        Windows11 = Windows | 8
    };

    /** Returns the type of operating system we're running on.

        @returns one of the values from the OperatingSystemType enum.
        @see getOperatingSystemName
    */
    static OperatingSystemType getOperatingSystemType();

    /** Returns the name of the type of operating system we're running on.

        @returns a string describing the OS type.
        @see getOperatingSystemType
    */
    static String getOperatingSystemName();

    /** Returns true if the OS is 64-bit, or false for a 32-bit OS. */
    static bool isOperatingSystem64Bit();

    /** Returns an environment variable.
        If the named value isn't set, this will return the defaultValue string instead.
    */
    static String getEnvironmentVariable(const String& name, const String& defaultValue);

    //==============================================================================
    /** Returns the current user's name, if available.
        @see getFullUserName()
    */
    static String getLogonName();

    /** Returns the current user's full name, if available.
        On some OSes, this may just return the same value as getLogonName().
        @see getLogonName()
    */
    static String getFullUserName();

    /** Returns the host-name of the computer. */
    static String getComputerName();

    /** This will attempt to return some kind of string describing the device.
        If no description is available, it'll just return an empty string. You may
        want to use this for things like determining the type of phone/iPad, etc.
    */
    static String getDeviceDescription();

    /** This will attempt to return the manufacturer of the device.
        If no description is available, it'll just return an empty string.
    */
    static String getDeviceManufacturer();

    //==============================================================================
    // CPU and memory information..

    /** Returns the number of logical CPU cores. */
    static int getNumCpus() noexcept;

    /** Returns the number of physical CPU cores. */
    static int getNumPhysicalCpus() noexcept;

    /** Returns the approximate CPU speed.
        @returns    the speed in megahertz, e.g. 1500, 2500, 32000 (depending on
                    what year you're reading this...)
    */
    static int getCpuSpeedInMegahertz();

    /** Returns a string to indicate the CPU vendor.
        Might not be known on some systems.
    */
    static String getCpuVendor();

    /** Attempts to return a string describing the CPU model.
        May not be available on some systems.
    */
    static String getCpuModel();

    static bool hasMMX() noexcept;   /**< Returns true if Intel MMX instructions are available. */
    static bool has3DNow() noexcept; /**< Returns true if AMD 3DNOW instructions are available. */
    static bool hasFMA3() noexcept;  /**< Returns true if AMD FMA3 instructions are available. */
    static bool hasFMA4() noexcept;  /**< Returns true if AMD FMA4 instructions are available. */
    static bool hasSSE() noexcept;   /**< Returns true if Intel SSE instructions are available. */
    static bool hasSSE2() noexcept;  /**< Returns true if Intel SSE2 instructions are available. */
    static bool hasSSE3() noexcept;  /**< Returns true if Intel SSE3 instructions are available. */
    static bool hasSSSE3() noexcept; /**< Returns true if Intel SSSE3 instructions are available. */
    static bool
    hasSSE41() noexcept; /**< Returns true if Intel SSE4.1 instructions are available. */
    static bool
    hasSSE42() noexcept;            /**< Returns true if Intel SSE4.2 instructions are available. */
    static bool hasAVX() noexcept;  /**< Returns true if Intel AVX instructions are available. */
    static bool hasAVX2() noexcept; /**< Returns true if Intel AVX2 instructions are available. */
    static bool hasAVX512F() noexcept;    /**< Returns true if Intel AVX-512 Foundation instructions
                                             are available. */
    static bool hasAVX512BW() noexcept;   /**< Returns true if Intel AVX-512 Byte and Word
                                             instructions are available. */
    static bool hasAVX512CD() noexcept;   /**< Returns true if Intel AVX-512 Conflict Detection
                                             instructions are available. */
    static bool hasAVX512DQ() noexcept;   /**< Returns true if Intel AVX-512 Doubleword and Quadword
                                             instructions are available. */
    static bool hasAVX512ER() noexcept;   /**< Returns true if Intel AVX-512 Exponential and
                                             Reciprocal instructions are available. */
    static bool hasAVX512IFMA() noexcept; /**< Returns true if Intel AVX-512 Integer Fused
                                             Multiply-Add instructions are available. */
    static bool hasAVX512PF() noexcept; /**< Returns true if Intel AVX-512 Prefetch instructions are
                                           available. */
    static bool hasAVX512VBMI() noexcept; /**< Returns true if Intel AVX-512 Vector Bit Manipulation
                                             instructions are available. */
    static bool hasAVX512VL() noexcept;   /**< Returns true if Intel AVX-512 Vector Length
                                             instructions are available. */
    static bool
    hasAVX512VPOPCNTDQ() noexcept; /**< Returns true if Intel AVX-512 Vector Population Count Double
                                      and Quad-word instructions are available. */
    static bool hasNeon() noexcept; /**< Returns true if ARM NEON instructions are available. */

    //==============================================================================
    /** Finds out how much RAM is in the machine.
        @returns    the approximate number of megabytes of memory, or zero if
                    something goes wrong when finding out.
    */
    static int getMemorySizeInMegabytes();

    /** Returns the system page-size.
        This is only used by programmers with beards.
    */
    static int getPageSize();

private:
    SystemStats() = delete; // uses only static methods
    GLUE_DECLARE_NON_COPYABLE(SystemStats)
};

} // namespace glue
