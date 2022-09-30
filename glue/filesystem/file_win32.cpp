//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/filesystem/file.h"
#include "glue/filesystem/path.h"

#include <windows.h>

namespace glue {
namespace fs {

File::File(const std::string &path) : m_path(path), m_fileInfo(nullptr) {}

File::~File() {
    if (m_fileInfo) {
        delete (WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo;
    }
}

std::string File::path() const { return m_path; }

std::string File::fileName() const { return Path(m_path).fileName(); }

bool File::exists() const {
    readFileInfo();

    return (m_fileInfo != nullptr);
}

bool File::isFile() const {
    readFileInfo();

    if (m_fileInfo) {
        return (((WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo)->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    return false;
}

bool File::isDirectory() const {
    readFileInfo();

    if (m_fileInfo) {
        return (((WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo)->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    return false;
}

bool File::isSymbolicLink() const {
    readFileInfo();

    if (m_fileInfo) {
        return (((WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo)->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
    }

    return false;
}

uint64_t File::size() const {
    readFileInfo();

    if (m_fileInfo) {
        LARGE_INTEGER li;
        li.LowPart = ((WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo)->nFileSizeLow;
        li.HighPart = ((WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo)->nFileSizeHigh;
        return li.QuadPart;
    }

    return 0;
}

bool File::copy(const std::string &path) const {
    // Check source file
    if (!isFile())
        return false;

    // Get source and target filenames
    Path dest(path);
    if (dest.isDirectory()) {
        std::string filename = Path(m_path).fileName();
        dest.resolve(filename);
    }

    std::string dst = dest.fullPath();

    // Copy file
    if (!CopyFileA(m_path.c_str(), dst.c_str(), FALSE)) {
        // Error!
        return false;
    }

    // Done
    return true;
}

bool File::move(const std::string &path) {
    // Check source file
    if (!exists())
        return false;

    // Get source and target filenames
    Path dest(path);
    if (dest.isDirectory()) {
        std::string filename = Path(m_path).fileName();
        dest.resolve(filename);
    }

    std::string dst = dest.fullPath();

    // Move file
    if (!MoveFileA(m_path.c_str(), dst.c_str())) {
        // Error!
        return false;
    }

    // Update path
    m_path = dst;
    updateFileInfo();

    // Done
    return true;
}

bool File::rename(const std::string &filename) {
    // Check file
    if (!exists())
        return false;

    // Compose new file path
    std::string path = Path(Path(m_path).directoryPath()).resolve(filename).fullPath();

    // Rename
    if (!MoveFileA(m_path.c_str(), path.c_str())) {
        // Error!
        return false;
    }

    // Update path
    m_path = path;
    updateFileInfo();

    // Done
    return true;
}

bool File::remove() {
    // Check source file
    if (!isFile())
        return false;

    // Delete file
    if (!DeleteFileA(m_path.c_str())) {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

bool File::createDirectory() {
    // Check directory
    if (exists())
        return false;

    // Create directory
    if (!CreateDirectoryA(m_path.c_str(), nullptr)) {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

bool File::removeDirectory() {
    // Check directory
    if (!isDirectory())
        return false;

    // Remove directory
    if (!RemoveDirectoryA(m_path.c_str())) {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

std::vector<std::string> File::listFiles() const {
    std::vector<std::string> entries;

    // Open directory
    WIN32_FIND_DATAA findData;
    std::string query = Path(m_path).fullPath() + "/*";
    HANDLE findHandle = FindFirstFileA(query.c_str(), &findData);

    if (findHandle == INVALID_HANDLE_VALUE) {
        return entries;
    }

    // Read directory entries
    do {
        // Get name
        std::string name = findData.cFileName;

        // Ignore . and ..
        if (name != ".." && name != ".") {
            entries.push_back(name);
        }
    } while (FindNextFileA(findHandle, &findData));

    // Close directory
    FindClose(findHandle);

    // Return directory entries
    return entries;
}

void File::updateFileInfo() {
    // Reset file information
    if (m_fileInfo) {
        delete (WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo;
        m_fileInfo = nullptr;
    }
}

void File::readFileInfo() const {
    // Check if file info has already been read
    if (m_fileInfo)
        return;

    // Create file information structure
    m_fileInfo = (void *)new WIN32_FILE_ATTRIBUTE_DATA;

    // Get file info
    if (!GetFileAttributesExA(m_path.c_str(), GetFileExInfoStandard, (WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo)) {
        // Error!
        delete (WIN32_FILE_ATTRIBUTE_DATA *)m_fileInfo;
        m_fileInfo = nullptr;
    }
}

} // namespace fs
} // namespace glue
