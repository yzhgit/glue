//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/file_mapping.h"

#include "glue/base/log.h"

namespace glue {

#if defined(GLUE_OS_WINDOWS)

FileMapping::FileMapping(const char* filename,
                         uint32_t permission,
                         uint64_t offset,
                         uint64_t length) {
    DWORD flags = 0;
    if (permission & FileMapping::READ) {
        flags |= GENERIC_READ;
    }
    if (permission & FileMapping::WRITE) {
        flags |= GENERIC_WRITE;
    }

    auto h = CreateFile(
        filename, flags, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h != INVALID_HANDLE_VALUE) {
        m_fileHandle = (void*)h;

        DWORD file_size = GetFileSize(h, nullptr);
        if (file_size == 0) {
            LogError("", "model file size is 0.");
            return;
        }

        if (offset >= file_size) {
            LogError("", "offset[%llu] >= file size[%lu]\n", offset, file_size);
            return;
        }

        auto max_length = file_size - offset;
        if (length > max_length) {
            length = max_length;
        }

        flags = 0;
        if (permission & FileMapping::WRITE) {
            flags = PAGE_READWRITE;
        } else if (permission & FileMapping::READ) {
            flags = PAGE_READONLY;
        }

        auto mappingHandle = CreateFileMapping(h, nullptr, flags, 0, 0, nullptr);
        if (mappingHandle != nullptr) {
            SYSTEM_INFO
            sys_info;  // system information; used to get granularity
            GetSystemInfo(&sys_info);

            uint64_t mapping_start_offset =
                (offset / sys_info.dwAllocationGranularity) * sys_info.dwAllocationGranularity;
            DWORD file_offset_high = (mapping_start_offset >> 32),
                  file_offset_low = (mapping_start_offset & 0xffffffff);

            flags = 0;
            if (permission & FileMapping::READ) {
                flags |= FILE_MAP_READ;
            }
            if (permission & FileMapping::WRITE) {
                flags |= FILE_MAP_WRITE;
            }

            m_base = MapViewOfFile(mappingHandle, flags, file_offset_high, file_offset_low, length);
            if (m_base != nullptr) {
                m_start = (char*)m_base + (offset - mapping_start_offset);
                m_size = length;
            }

            CloseHandle(mappingHandle);
        }
    }
}

FileMapping::~FileMapping() {
    if (m_base != nullptr) UnmapViewOfFile(m_base);

    if (m_fileHandle != nullptr) CloseHandle((HANDLE)m_fileHandle);
}

#else

FileMapping::FileMapping(const char* filename,
                         uint32_t permission,
                         uint64_t offset,
                         uint64_t length) {
    int flags = O_CLOEXEC;
    if ((permission & FileMapping::READ) && (permission & FileMapping::WRITE)) {
        flags |= O_RDWR;
    } else if (permission & FileMapping::WRITE) {
        flags |= O_WRONLY;
    } else if (permission & FileMapping::READ) {
        flags |= O_RDONLY;
    }

    int fd = open(filename, flags);
    if (fd != -1) {
        m_fileHandle = fd;

        struct stat file_stat_info;
        memset(&file_stat_info, 0, sizeof(file_stat_info));
        if (fstat(fd, &file_stat_info) < 0 || file_stat_info.st_size < 0) {
            LogError("", "model file size is 0.");
            return;
        }

        uint64_t file_size = file_stat_info.st_size;
        if (offset >= file_size) {
            LogError("", "offset[%llu] >= file size[%lu]\n", offset, file_size);
            return;
        }

        auto max_length = file_size - offset;
        if (length > max_length) {
            length = max_length;
        }

        auto page_size = sysconf(_SC_PAGE_SIZE);
        auto mapping_start_offset = (offset / page_size) * page_size;

        flags = 0;
        if (permission & FileMapping::READ) {
            flags |= PROT_READ;
        }
        if (permission & FileMapping::WRITE) {
            flags |= PROT_WRITE;
        }

        m_base = mmap(NULL, length, flags, MAP_PRIVATE, fd, mapping_start_offset);
        if (m_base != MAP_FAILED) {
            m_start = (char*)m_base + (offset - mapping_start_offset);
            m_size = length;
        }
    }
}

FileMapping::~FileMapping() {
    if (m_base != nullptr) munmap(m_base, (size_t)m_size);

    if (m_fileHandle != 0) close(m_fileHandle);
}

#endif

}  // namespace glue
