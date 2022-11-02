//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/String.h"

namespace glue
{

//==============================================================================
/**
    Represents a local file or directory.

    This class encapsulates the absolute pathname of a file or directory, and
    has methods for finding out about the file and changing its properties.

    To read or write to the file, there are methods for returning an input or
    output stream.

    @see FileInputStream, FileOutputStream

    @tags{Core}
*/
class GLUE_API File final
{
public:
    enum MemoryMapFlags
    {
        NoOptions,
        MapPrivateOption
    };

    //==============================================================================
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

    /** Destructor. */
    ~File() = default;

    /** Creates a copy of another file object. */
    File(const File&);

    /** Copies from another file object. */
    File& operator=(const File& otherFile);

    /** Move constructor */
    File(File&&) noexcept;

    /** Move assignment operator */
    File& operator=(File&&) noexcept;

    //==============================================================================
    uchar* map(int64 offset, int64 size, MemoryMapFlags flags = NoOptions);
    bool unmap(uchar* address);

    //==============================================================================
    /** Checks whether the file actually exists.

        @returns    true if the file exists, either as a file or a directory.
        @see isFile, isDirectory
    */
    bool exists() const;

    /** Checks whether the file exists and is a file rather than a directory.

        @returns    true only if this is a real file, false if it's a directory
                    or doesn't exist
        @see exists, isDirectory
    */
    bool isFile() const;

    /** Checks whether the file is a directory that exists.

        @returns    true only if the file is a directory which actually exists, so
                    false if it's a file or doesn't exist at all
        @see exists, isFile
    */
    bool isDirectory() const;

    /** Checks whether the path of this file represents the root of a file system,
        irrespective of its existence.

        This will return true for "C:", "D:", etc on Windows and "/" on other
        platforms.
    */
    bool isRoot() const;

    /** Returns the size of the file in bytes.

        @returns    the number of bytes in the file, or 0 if it doesn't exist.
    */
    int64 getSize() const;

    //==============================================================================
    /** Returns the complete, absolute path of this file.

        This includes the filename and all its parent folders. On Windows it'll
        also include the drive letter prefix; on Mac or Linux it'll be a complete
        path starting from the root folder.

        If you just want the file's name, you should use getFileName() or
        getFileNameWithoutExtension().

        @see getFileName
    */
    const std::string& getFullPathName() const noexcept { return m_fullPath; }

    /** Returns the last section of the pathname.

        Returns just the final part of the path - e.g. if the whole path
        is "/moose/fish/foo.txt" this will return "foo.txt".

        For a directory, it returns the final part of the path - e.g. for the
        directory "/moose/fish" it'll return "fish".

        If the filename begins with a dot, it'll return the whole filename, e.g. for
        "/moose/.fish", it'll return ".fish"

        @see getFullPathName, getFileNameWithoutExtension
    */
    std::string getFileName() const;

    //==============================================================================
    /** Returns the file's extension.

        Returns the file extension of this file, also including the dot.

        e.g. "/moose/fish/foo.txt" would return ".txt"

        @see hasFileExtension, setFileExtension, getFileNameWithoutExtension
    */
    std::string getFileExtension() const;

    /** Checks whether the file has a given extension.

        @param extensionToTest  the extension to look for - it doesn't matter whether or
                                not this string has a dot at the start, so ".wav" and "wav"
                                will have the same effect. To compare with multiple extensions, this
                                parameter can contain multiple strings, separated by semi-colons -
                                so, for example: hasFileExtension (".jpeg;png;gif") would return
                                true if the file has any of those three extensions.

        @see getFileExtension, setFileExtension, getFileNameWithoutExtension
    */
    bool hasFileExtension(StringRef extensionToTest) const;

    /** Returns a version of this file with a different file extension.

        e.g. File ("/moose/fish/foo.txt").setFileExtension ("html") returns "/moose/fish/foo.html"

        @param newExtension     the new extension, either with or without a dot at the start (this
                                doesn't make any difference). To get remove a file's extension
       altogether, pass an empty string into this function.

        @see getFileName, getFileExtension, hasFileExtension, getFileNameWithoutExtension
    */
    File setFileExtension(StringRef newExtension) const;

    /** Returns the last part of the filename, without its file extension.

        e.g. for "/moose/fish/foo.txt" this will return "foo".

        @see getFileName, getFileExtension, hasFileExtension, setFileExtension
    */
    std::string getFileNameWithoutExtension() const;

    //==============================================================================
    /** Returns a file that represents a relative (or absolute) sub-path of the current one.

        This will find a child file or directory of the current object.

        e.g.
            File ("/moose/fish").getChildFile ("foo.txt") will produce "/moose/fish/foo.txt".
            File ("/moose/fish").getChildFile ("haddock/foo.txt") will produce
       "/moose/fish/haddock/foo.txt". File ("/moose/fish").getChildFile ("../foo.txt") will produce
       "/moose/foo.txt".

        If the string is actually an absolute path, it will be treated as such, e.g.
            File ("/moose/fish").getChildFile ("/foo.txt") will produce "/foo.txt"

        @see getSiblingFile, getParentDirectory
    */
    File getChildFile(StringRef relativeOrAbsolutePath) const;

    /** Returns a file which is in the same directory as this one.

        This is equivalent to getParentDirectory().getChildFile (name).

        @see getChildFile, getParentDirectory
    */
    File getSiblingFile(StringRef siblingFileName) const;

    //==============================================================================
    /** Returns the directory that contains this file or directory.

        e.g. for "/moose/fish/foo.txt" this will return "/moose/fish".

        If you are already at the root directory ("/" or "C:") then this method will
        return the root directory.
    */
    File getParentDirectory() const;

    //==============================================================================
    /** Chooses a filename relative to this one that doesn't already exist.

        If this file is a directory, this will return a child file of this
        directory that doesn't exist, by adding numbers to a prefix and suffix until
        it finds one that isn't already there.

        If the prefix + the suffix doesn't exist, it won't bother adding a number.

        e.g. File ("/moose/fish").getNonexistentChildFile ("foo", ".txt", true) might
             return "/moose/fish/foo(2).txt" if there's already a file called "foo.txt".

        @param prefix                   the string to use for the filename before the number
        @param suffix                   the string to add to the filename after the number
        @param putNumbersInBrackets     if true, this will create filenames in the
                                        format "prefix(number)suffix", if false, it will leave the
                                        brackets out.
    */
    File getNonexistentChildFile(const std::string& prefix, const std::string& suffix,
                                 bool putNumbersInBrackets = true) const;

    /** Chooses a filename for a sibling file to this one that doesn't already exist.

        If this file doesn't exist, this will just return itself, otherwise it
        will return an appropriate sibling that doesn't exist, e.g. if a file
        "/moose/fish/foo.txt" exists, this might return "/moose/fish/foo(2).txt".

        @param putNumbersInBrackets     whether to add brackets around the numbers that
                                        get appended to the new filename.
    */
    File getNonexistentSibling(bool putNumbersInBrackets = true) const;

    /** Compares the pathnames for two files. */
    bool operator==(const File&) const;
    /** Compares the pathnames for two files. */
    bool operator!=(const File&) const;

    //==============================================================================
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
    bool create() const;

    /** Creates a new directory for this filename.

        This will try to create the file as a directory, and will also create
        any parent directories it needs in order to complete the operation.

        @returns    a result to indicate whether the directory was created successfully, or
                    an error message if it failed.
        @see create
    */
    bool createDirectory() const;

    /** Deletes a file.

        If this file is actually a directory, it may not be deleted correctly if it
        contains files. See deleteRecursively() as a better way of deleting directories.

        If this file is a symlink, then the symlink will be deleted and not the target
        of the symlink.

        @returns    true if the file has been successfully deleted (or if it didn't exist to
                    begin with).
        @see deleteRecursively
    */
    bool deleteFile() const;

    /** Moves or renames a file.

        Tries to move a file to a different location.
        If the target file already exists, this will attempt to delete it first, and
        will fail if this can't be done.

        Note that the destination file isn't the directory to put it in, it's the actual
        filename that you want the new file to have.

        Also note that on some OSes (e.g. Windows), moving files between different
        volumes may not be possible.

        @returns    true if the operation succeeds
    */
    bool moveFileTo(const File& targetLocation) const;

    /** Copies a file.

        Tries to copy a file to a different location. If the target file already exists,
        this will attempt to delete it first, and will fail if this can't be done.

        Note that the target file isn't the directory to put it in, it's the actual
        filename that you want the new file to have.

        @returns    true if the operation succeeds
    */
    bool copyFileTo(const File& targetLocation) const;

    /** Replaces a file.

        Replace the file in the given location, assuming the replaced files identity.
        Depending on the file system this will preserve file attributes such as
        creation date, short file name, etc.

        If replacement succeeds the original file is deleted.

        @returns    true if the operation succeeds
    */
    bool replaceFileIn(const File& targetLocation) const;

    //==============================================================================
    /** Used in file searching, to specify whether to return files, directories, or both.
     */
    enum TypesOfFileToFind
    {
        findDirectories = 1, /**< Use this flag to indicate that you want to find directories. */
        findFiles = 2,       /**< Use this flag to indicate that you want to find files. */
        findFilesAndDirectories =
            3, /**< Use this flag to indicate that you want to find both files and directories. */
        ignoreHiddenFiles =
            4 /**< Add this flag to avoid returning any hidden files in the results. */
    };

    enum class FollowSymlinks
    {
        /** Requests that a file system traversal should not follow any symbolic links. */
        no,

        /** Requests that a file system traversal may follow symbolic links, but should attempt to
            skip any symbolic links to directories that may cause a cycle.
        */
        noCycles,

        /** Requests that a file system traversal follow all symbolic links. Use with care, as this
            may produce inconsistent results, or fail to terminate, if the filesystem contains
           cycles due to symbolic links.
        */
        yes
    };

    //==============================================================================
    /** A set of types of location that can be passed to the getSpecialLocation() method.
     */
    enum SpecialLocationType
    {
        /** The user's home folder. This is the same as using File ("~"). */
        userHomeDirectory,

        /** The user's default documents folder. On Windows, this might be the user's
            "My Documents" folder. On the Mac it'll be their "Documents" folder. Linux
            doesn't tend to have one of these, so it might just return their home folder.
        */
        userDocumentsDirectory,

        /** The folder that contains the user's desktop objects. */
        userDesktopDirectory,

        /** The most likely place where a user might store their music files. */
        userMusicDirectory,

        /** The most likely place where a user might store their movie files. */
        userMoviesDirectory,

        /** The most likely place where a user might store their picture files. */
        userPicturesDirectory,

        /** The folder in which applications store their persistent user-specific settings.
            On Windows, this might be "\Documents and Settings\username\Application Data".
            On the Mac, it might be "~/Library". If you're going to store your settings in here,
            always create your own sub-folder to put them in, to avoid making a mess.
            On GNU/Linux it is "~/.config".
        */
        userApplicationDataDirectory,

        /** An equivalent of the userApplicationDataDirectory folder that is shared by all users
            of the computer, rather than just the current user.

            On the Mac it'll be "/Library", on Windows, it could be something like
            "\Documents and Settings\All Users\Application Data".

            On GNU/Linux it is "/opt".

            Depending on the setup, this folder may be read-only.
        */
        commonApplicationDataDirectory,

        /** A place to put documents which are shared by all users of the machine.
            On Windows this may be somewhere like "C:\Users\Public\Documents", on OSX it
            will be something like "/Users/Shared". Other OSes may have no such concept
            though, so be careful.
        */
        commonDocumentsDirectory,

        /** The folder that should be used for temporary files.
            Always delete them when you're finished, to keep the user's computer tidy!
        */
        tempDirectory,

        /** Returns this application's executable file.

            If running as a plug-in or DLL, this will (where possible) be the DLL rather than the
            host app.

            On the mac this will return the unix binary, not the package folder - see
            currentApplicationFile for that.

            See also invokedExecutableFile, which is similar, but if the exe was launched from a
            file link, invokedExecutableFile will return the name of the link.
        */
        currentExecutableFile,

        /** Returns this application's location.

            If running as a plug-in or DLL, this will (where possible) be the DLL rather than the
            host app.

            On the mac this will return the package folder (if it's in one), not the unix binary
            that's inside it - compare with currentExecutableFile.
        */
        currentApplicationFile,

        /** Returns the file that was invoked to launch this executable.
            This may differ from currentExecutableFile if the app was started from e.g. a link -
           this will return the name of the link that was used, whereas currentExecutableFile will
           return the actual location of the target executable.
        */
        invokedExecutableFile,

        /** In a plugin, this will return the path of the host executable. */
        hostApplicationPath,

#if GLUE_WINDOWS || DOXYGEN
        /** On a Windows machine, returns the location of the Windows/System32 folder. */
        windowsSystemDirectory,
#endif

        /** The directory in which applications normally get installed.
            So on windows, this would be something like "C:\Program Files", on the
            Mac "/Applications", or "/usr" on linux.
        */
        globalApplicationsDirectory,

#if GLUE_WINDOWS || DOXYGEN
        /** On a Windows machine, returns the directory in which 32 bit applications
            normally get installed. On a 64 bit machine this would be something like
            "C:\Program Files (x86)", whereas for 32 bit machines this would match
            globalApplicationsDirectory and be something like "C:\Program Files".

            @see globalApplicationsDirectory
        */
        globalApplicationsDirectoryX86,

        /** On a Windows machine returns the %LOCALAPPDATA% folder. */
        windowsLocalAppData
#endif
    };

    /** Finds the location of a special type of file or directory, such as a home folder or
        documents folder.

        @see SpecialLocationType
    */
    static File GLUE_CALLTYPE getSpecialLocation(const SpecialLocationType type);

    //==============================================================================
    /** Returns the current working directory.
        @see setAsCurrentWorkingDirectory
    */
    static File getCurrentWorkingDirectory();

    /** Sets the current working directory to be this file.

        For this to work the file must point to a valid directory.

        @returns true if the current directory has been changed.
        @see getCurrentWorkingDirectory
    */
    bool setAsCurrentWorkingDirectory() const;

    //==============================================================================
    /** The system-specific file separator character.
        On Windows, this will be '\', on Mac/Linux, it'll be '/'
    */
    static glue_wchar getSeparatorChar();

    /** The system-specific file separator character, as a string.
        On Windows, this will be '\', on Mac/Linux, it'll be '/'
    */
    static StringRef getSeparatorString();

    /** Returns true if the string seems to be a fully-specified absolute path. */
    static bool isAbsolutePath(StringRef path);

    /** Creates a file that simply contains this string, without doing the sanity-checking
        that the normal constructors do.

        Best to avoid this unless you really know what you're doing.
    */
    static File createFileWithoutCheckingPath(const std::string& absolutePath) noexcept;

    /** Adds a separator character to the end of a path if it doesn't already have one. */
    static std::string addTrailingSeparator(const std::string& path);

    /** Returns true if this file is a link.
     */
    bool isSymbolicLink() const;

private:
    //==============================================================================
    std::string m_fullPath;

    static std::string parseAbsolutePath(const std::string&);
    std::string getPathUpToLastSlash() const;

    bool createDirectoryInternal(const std::string&) const;
    bool copyInternal(const File&) const;
    bool moveInternal(const File&) const;
    bool replaceInternal(const File&) const;
    bool setFileTimesInternal(int64 m, int64 a, int64 c) const;
    void getFileTimesInternal(int64& m, int64& a, int64& c) const;
    bool setFileReadOnlyInternal(bool) const;
    bool setFileExecutableInternal(bool) const;
};

} // namespace glue
