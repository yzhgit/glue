//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/leaked_object_detector.h"
#include "glue/base/range.h"

#include "ghc/fs_std.hpp"

GLUE_START_NAMESPACE

//==============================================================================
/**
    Maps a file into virtual memory for easy reading and/or writing.

    @tags{Core}
*/
class GLUE_API FileMapping
{
public:
    static constexpr uint32_t READ = 1;
    static constexpr uint32_t WRITE = 2;

public:
    /** Opens a section of a file and maps it to an area of virtual memory.

        The file should already exist, and should already be the size that you want to work with
        when you call this. If the file is resized after being opened, the behaviour is undefined.

        If the file exists and the operation succeeds, the getData() and getSize() methods will
        return the location and size of the data that can be read or written. Note that the entire
        file is not read into memory immediately - the OS simply creates a virtual mapping, which
        will lazily pull the data into memory when blocks are accessed.

        If the file can't be opened for some reason, the getData() method will return a null
       pointer.

        NOTE: The start of the actual range used may be rounded-down to a multiple of the OS's
       page-size, so do not assume that the mapped memory will begin at exactly the position you
       requested.
    */
    FileMapping(const char* filename, uint32_t permission, uint64_t offset = 0,
                uint64_t length = UINT64_MAX);

    /** Destructor. */
    ~FileMapping();

    /** Returns the address at which this file has been mapped, or a null pointer if
        the file couldn't be successfully mapped.
    */
    char* getData() noexcept
    {
        return static_cast<char*>(m_start);
    }

    const char* getData() const noexcept
    {
        return static_cast<char*>(m_start);
    }

    /** Returns the number of bytes of data that are available for reading or writing.
        This will normally be the size of the file.
    */
    uint64_t getSize() const noexcept
    {
        return m_size;
    }

private:
#if defined(GLUE_OS_WINDOWS)
    void* m_fileHandle{nullptr};
#else
    int m_fileHandle{0};
#endif

    void* m_base{nullptr};
    void* m_start{nullptr};
    uint64_t m_size{0};

    GLUE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileMapping)
};

GLUE_END_NAMESPACE
