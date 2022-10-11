//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <string>
#include <vector>

namespace glue
{
namespace fs
{

    class File
    {
    public:
        /** Creates an (invalid) file object.

            The file is initially set to an empty path, so getFullPathName() will return
            an empty string.

            You can use its operator= method to point it at a proper file.
        */
        File() = default;

        /** Creates a file from an absolute path.

            If the path supplied is a relative path, it is taken to be relative
            to the current working directory (see File::getCurrentWorkingDirectory()),
            but this isn't a recommended way of creating a file, because you
            never know what the CWD is going to be.

            On the Mac/Linux, the path can include "~" notation for referring to
            user home directories.
        */
        File(const std::string& absolutePath);

        /** Creates a copy of another file object. */
        File(const File&);

        /** Destructor. */
        ~File() = default;

        /** Sets the file based on an absolute pathname.

            If the path supplied is a relative path, it is taken to be relative
            to the current working directory (see File::getCurrentWorkingDirectory()),
            but this isn't a recommended way of creating a file, because you
            never know what the CWD is going to be.

            On the Mac/Linux, the path can include "~" notation for referring to
            user home directories.
        */
        File& operator=(const std::string& newAbsolutePath);

        /** Copies from another file object. */
        File& operator=(const File& otherFile);

        /** Move constructor */
        File(File&&) noexcept;

        /** Move assignment operator */
        File& operator=(File&&) noexcept;

        /**
         *  @brief
         *    Get path
         *
         *  @return
         *    Path to file or directory
         */
        std::string path() const;

        /**
         *  @brief
         *    Get filename
         *
         *  @return
         *    Filename
         */
        std::string fileName() const;

        /** Checks whether the file actually exists.

            @returns    true if the file exists, either as a file or a directory.
            @see existsAsFile, isDirectory
        */
        bool exists() const;

        /** Checks whether the file is a regular file that exists.

            @returns    true only if the file is a regular file which actually exists, so
                        false if it's a directory or doesn't exist at all
            @see exists
        */
        bool isFile() const;

        /** Checks whether the file is a directory that exists.

            @returns    true only if the file is a directory which actually exists, so
                        false if it's a file or doesn't exist at all
            @see exists
        */
        bool isDirectory() const;

        /**
         *  @brief
         *    Check if item is a symbolic link
         *
         *  @return
         *    'true' if it is a symbolic link, else 'false'
         */
        bool isSymbolicLink() const;

        /** Returns the size of the file in bytes.

            @returns    the number of bytes in the file, or 0 if it doesn't exist.
        */
        uint64_t size() const;

        /// Copies the file (or directory) to the given path.
        /// The target path can be a directory.
        ///
        /// A directory is copied recursively.
        /// If options is set to OPT_FAIL_ON_OVERWRITE the Method throws an FileExists Exception
        /// if the File already exists.
        bool copy(const std::string& path) const;

        /// Copies the file (or directory) to the given path and
        /// removes the original file. The target path can be a directory.
        /// If options is set to OPT_FAIL_ON_OVERWRITE the Method throws an FileExists Exception
        /// if the File already exists.
        bool move(const std::string& path);

        /// Renames the file to the new name.
        /// If options is set to OPT_FAIL_ON_OVERWRITE the Method throws an FileExists Exception
        /// if the File already exists.
        bool rename(const std::string& path);

        /// Deletes the file. If recursive is true and the
        /// file is a directory, recursively deletes all
        /// files in the directory.
        bool remove();

        /** Creates an empty file if it doesn't already exist.

            If the file that this object refers to doesn't exist, this will create a file
            of zero size.

            If it already exists or is a directory, this method will do nothing.

            If the parent directories of the File do not exist then this method will
            recursively create the parent directories.

            @returns    a result to indicate whether the file was created successfully,
                        or an error message if it failed.
            @see createDirectory
        */
        bool createFile();

        /** Creates a new directory for this filename.

            This will try to create the file as a directory, and will also create
            any parent directories it needs in order to complete the operation.

            @returns    a result to indicate whether the directory was created successfully, or
                        an error message if it failed.
            @see create
        */
        bool createDirectory();

        /// Creates a directory (and all parent directories
        /// if necessary).
        void createDirectories();

        /**
         *  @brief
         *    Remove directory
         *
         *  @return
         *    'true' if successful, else 'false'
         *
         *  @remarks
         *    Only works if the directory exists and is empty.
         *    To remove an entire directory tree, see removeDirectoryRec.
         */
        bool removeDirectory();

        /**
         *  @brief
         *    List files in directory
         *
         *  @return
         *    List of files, empty list if this is not a valid directory
         */
        std::vector<std::string> listFiles() const;

    private:
        void updateFileInfo();
        void readFileInfo() const;

    private:
        std::string m_path; ///< Path to file or directory
        mutable void*
            m_fileInfo; ///< Information about the current file (resolves links, created on demand)
    };

} // namespace fs
} // namespace glue
