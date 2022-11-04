//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/MemoryMappedFile.h"

GLUE_START_NAMESPACE

//==============================================================================
MemoryMappedFile::MemoryMappedFile(const fs::path& path, MemoryMappedFile::AccessMode mode,
                                   bool exclusive)
    : m_range(0, fs::file_size(path))
{
    openInternal(path, mode, exclusive);
}

MemoryMappedFile::MemoryMappedFile(const fs::path& path, const Range<int64>& fileRange,
                                   AccessMode mode, bool exclusive)
    : m_range(fileRange.getIntersectionWith(Range<int64>(0, fs::file_size(path))))
{
    openInternal(path, mode, exclusive);
}

#if defined(GLUE_OS_WINDOWS)

//==============================================================================
void MemoryMappedFile::openInternal(const fs::path& path, AccessMode mode, bool exclusive)
{
    if (m_range.getStart() > 0)
    {
        SYSTEM_INFO systemInfo;
        GetNativeSystemInfo(&systemInfo);

        m_range.setStart(m_range.getStart() -
                         (m_range.getStart() % systemInfo.dwAllocationGranularity));
    }

    DWORD accessMode = GENERIC_READ, createType = OPEN_EXISTING;
    DWORD protect = PAGE_READONLY, access = FILE_MAP_READ;

    if (mode == readWrite)
    {
        accessMode = GENERIC_READ | GENERIC_WRITE;
        createType = OPEN_ALWAYS;
        protect = PAGE_READWRITE;
        access = FILE_MAP_ALL_ACCESS;
    }

    auto h = CreateFile(
        path.generic_u8string().c_str(), accessMode,
        exclusive
            ? 0
            : (FILE_SHARE_READ | FILE_SHARE_DELETE | (mode == readWrite ? FILE_SHARE_WRITE : 0)),
        nullptr, createType, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);

    if (h != INVALID_HANDLE_VALUE)
    {
        m_fileHandle = (void*) h;

        auto mappingHandle =
            CreateFileMapping(h, nullptr, protect, (DWORD) (m_range.getEnd() >> 32),
                              (DWORD) m_range.getEnd(), nullptr);

        if (mappingHandle != nullptr)
        {
            m_address = MapViewOfFile(mappingHandle, access, (DWORD) (m_range.getStart() >> 32),
                                      (DWORD) m_range.getStart(), (SIZE_T) m_range.getLength());

            if (m_address == nullptr) m_range = Range<int64>();

            CloseHandle(mappingHandle);
        }
    }
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (m_address != nullptr) UnmapViewOfFile(m_address);

    if (m_fileHandle != nullptr) CloseHandle((HANDLE) m_fileHandle);
}

#else

void MemoryMappedFile::openInternal(const fs::path& path, AccessMode mode, bool exclusive)
{
    GLUE_ASSERT(mode == readOnly || mode == readWrite);

    if (m_range.getStart() > 0)
    {
        auto pageSize = sysconf(_SC_PAGE_SIZE);
        m_range.setStart(m_range.getStart() - (m_range.getStart() % pageSize));
    }

    auto filename = path.c_str();

    if (mode == readWrite)
        m_fileHandle = open(filename, O_CREAT | O_RDWR, 00644);
    else
        m_fileHandle = open(filename, O_RDONLY);

    if (m_fileHandle != -1)
    {
        auto m =
            mmap(nullptr, (size_t) m_range.getLength(),
                 mode == readWrite ? (PROT_READ | PROT_WRITE) : PROT_READ,
                 exclusive ? MAP_PRIVATE : MAP_SHARED, m_fileHandle, (off_t) m_range.getStart());

        if (m != MAP_FAILED)
        {
            m_address = m;
            madvise(m, (size_t) m_range.getLength(), MADV_SEQUENTIAL);
        }
        else
        {
            m_range = Range<int64>();
        }

        close(m_fileHandle);
        m_fileHandle = 0;
    }
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (m_address != nullptr) munmap(m_address, (size_t) m_range.getLength());

    if (m_fileHandle != 0) close(m_fileHandle);
}

#endif

GLUE_END_NAMESPACE
