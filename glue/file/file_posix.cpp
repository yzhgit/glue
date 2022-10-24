//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/file/file.h"
#include "glue/file/path.h"

#include <dirent.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace glue
{
namespace fs
{

    File::File(const std::string& path) : m_path(path), m_fileInfo(nullptr) {}

    File::~File()
    {
        if (m_fileInfo) { delete (struct stat*) m_fileInfo; }
    }

    std::string File::path() const { return m_path; }

    std::string File::fileName() const { return Path(m_path).fileName(); }

    bool File::exists() const
    {
        readFileInfo();

        return (m_fileInfo != nullptr);
    }

    bool File::isFile() const
    {
        readFileInfo();

        if (m_fileInfo) { return S_ISREG(((struct stat*) m_fileInfo)->st_mode); }

        return false;
    }

    bool File::isDirectory() const
    {
        readFileInfo();

        if (m_fileInfo) { return S_ISDIR(((struct stat*) m_fileInfo)->st_mode); }

        return false;
    }

    bool File::isSymbolicLink() const
    {
        readLinkInfo();

        if (m_linkInfo) { return S_ISLNK(((struct stat*) m_linkInfo)->st_mode); }

        return false;
    }

    uint64_t File::size() const
    {
        readFileInfo();

        if (m_fileInfo)
        {
            if (S_ISREG(((struct stat*) m_fileInfo)->st_mode))
            {
                return ((struct stat*) m_fileInfo)->st_size;
            }
        }

        return 0;
    }

    bool File::copy(const std::string& path)
    {
        // Check source file
        if (!isFile()) return false;

        // Get source and target filenames
        Path dest(path);
        if (dest.isDirectory())
        {
            std::string filename = Path(m_path).fileName();
            dest.resolve(filename);
        }

        std::string dst = dest.fullPath();

        // Open files
        std::ifstream in(m_path.c_str(), std::ios::binary);
        std::ofstream out(dst.c_str(), std::ios::binary);
        if (!in || !out)
        {
            // Error!
            return false;
        }

        // Copy file
        out << in.rdbuf();

        // Done
        return true;
    }

    bool File::move(const std::string& path)
    {
        // Check source file
        if (!exists()) return false;

        // Get source and target filenames
        Path dest(path);
        if (dest.isDirectory())
        {
            std::string filename = Path(m_path).fileName();
            dest.resolve(filename);
        }

        std::string dst = dest.fullPath();

        // Move file
        if (::rename(m_path.c_str(), dst.c_str()) != 0) { return false; }

        // Update path
        m_path = dst;
        updateFileInfo();

        // Done
        return true;
    }

    bool File::rename(const std::string& filename)
    {
        // Check file
        if (!exists()) return false;

        // Compose new file path
        std::string path = Path(Path(m_path).directoryPath()).resolve(filename).fullPath();

        // Rename
        if (::rename(m_path.c_str(), path.c_str()) != 0) { return false; }

        // Update path
        m_path = path;
        updateFileInfo();

        // Done
        return true;
    }

    bool File::remove()
    {
        // Check source file
        if (!isFile()) return false;

        // Delete file
        if (::remove(m_path.c_str()) != 0) { return false; }

        // Done
        updateFileInfo();
        return true;
    }

    bool File::createDirectory()
    {
        // Check directory
        if (exists()) return false;

        // Create directory
        if (::mkdir(m_path.c_str(), 0755) != 0) { return false; }

        // Done
        updateFileInfo();
        return true;
    }

    bool File::removeDirectory()
    {
        // Check directory
        if (!isDirectory()) return false;

        // Remove directory
        if (::remove(m_path.c_str()) != 0) { return false; }

        // Done
        updateFileInfo();
        return true;
    }

    std::vector<std::string> File::listFiles() const
    {
        std::vector<std::string> entries;

        // Open directory
        DIR* dir = opendir(m_path.c_str());
        if (!dir) { return entries; }

        // Read directory entries
        struct dirent* entry = readdir(dir);
        while (entry)
        {
            // Get name
            std::string name = entry->d_name;

            // Ignore . and ..
            if (name != ".." && name != ".") { entries.push_back(name); }

            // Next entry
            entry = readdir(dir);
        }

        // Close directory
        closedir(dir);

        // Return directory entries
        return entries;
    }

    void File::updateFileInfo()
    {
        // Reset file information
        if (m_fileInfo)
        {
            delete (struct stat*) m_fileInfo;
            m_fileInfo = nullptr;
        }
    }

    void File::readFileInfo() const
    {
        // Check if file info has already been read
        if (m_fileInfo) return;

        // Create file information structure
        m_fileInfo = (void*) new struct stat;

        // Get file info
        if (stat(m_path.c_str(), (struct stat*) m_fileInfo) != 0)
        {
            // Error!
            delete (struct stat*) m_fileInfo;
            m_fileInfo = nullptr;
        }
    }

} // namespace fs
} // namespace glue
