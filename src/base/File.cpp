//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/File.h"

#include "glue/base/MemoryMappedFile.h"
#include "glue/base/MathsFunctions.h"
#include "glue/base/StringArray.h"

namespace glue
{

File::File(const std::string& fullPathName) : m_fullPath(parseAbsolutePath(fullPathName)) {}

File File::createFileWithoutCheckingPath(const std::string& path) noexcept
{
    File f;
    f.m_fullPath = path;
    return f;
}

File::File(const File& other) : m_fullPath(other.m_fullPath) {}

File& File::operator=(const std::string& newPath)
{
    m_fullPath = parseAbsolutePath(newPath);
    return *this;
}

File& File::operator=(const File& other)
{
    m_fullPath = other.m_fullPath;
    return *this;
}

File::File(File&& other) noexcept : m_fullPath(std::move(other.m_fullPath)) {}

File& File::operator=(File&& other) noexcept
{
    m_fullPath = std::move(other.m_fullPath);
    return *this;
}

//==============================================================================
static std::string removeEllipsis(const std::string& path)
{
    // This will quickly find both /../ and /./ at the expense of a minor
    // false-positive performance hit when path elements end in a dot.
#if GLUE_WINDOWS
    if (path.contains(".\\"))
#else
    if (path.contains("./"))
#endif
    {
        StringArray toks;
        toks.addTokens(path, File::getSeparatorString(), {});
        bool anythingChanged = false;

        for (int i = 1; i < toks.size(); ++i)
        {
            auto& t = toks[i];

            if (t == ".." && toks[i - 1] != "..")
            {
                anythingChanged = true;
                toks.removeRange(i - 1, 2);
                i = jmax(0, i - 2);
            }
            else if (t == ".")
            {
                anythingChanged = true;
                toks.remove(i--);
            }
        }

        if (anythingChanged) return toks.joinIntoString(File::getSeparatorString());
    }

    return path;
}

static std::string normaliseSeparators(const std::string& path)
{
    auto normalisedPath = path;

    std::string separator(File::getSeparatorString());
    std::string doubleSeparator(separator + separator);

    auto uncPath =
        normalisedPath.startsWith(doubleSeparator) &&
        !normalisedPath.fromFirstOccurrenceOf(doubleSeparator, false, false).startsWith(separator);

    if (uncPath)
        normalisedPath = normalisedPath.fromFirstOccurrenceOf(doubleSeparator, false, false);

    while (normalisedPath.contains(doubleSeparator))
        normalisedPath = normalisedPath.replace(doubleSeparator, separator);

    return uncPath ? doubleSeparator + normalisedPath : normalisedPath;
}

//==============================================================================
bool File::exists() const
{
    if (m_fullPath.isEmpty()) return false;

#if GLUE_WINDOWS
    DWORD attr = GetFileAttributesW(m_fullPath.toWideCharPointer());
    return (attr != INVALID_FILE_ATTRIBUTES);
#else
    struct stat st;
    return stat(m_fullPath.toUTF8(), &st) == 0;
#endif
}

bool File::isFile() const
{
    if (m_fullPath.isEmpty()) return false;

#if GLUE_WINDOWS
    DWORD attr = GetFileAttributesW(m_fullPath.toWideCharPointer());
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
    struct stat st;
    return (stat(m_fullPath.toUTF8(), &st) == 0) && S_ISREG(st.st_mode);
#endif
}

bool File::isDirectory() const
{
    if (m_fullPath.isEmpty()) return false;

#if GLUE_WINDOWS
    DWORD attr = GetFileAttributesW(m_fullPath.toWideCharPointer());
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
    struct stat st;
    return (stat(m_fullPath.toUTF8(), &st) == 0) && S_ISDIR(st.st_mode);
#endif
}

bool File::isRoot() const { return m_fullPath.isNotEmpty() && *this == getParentDirectory(); }

int64 File::getSize() const
{
    if (m_fullPath.isEmpty()) return 0;

#if GLUE_WINDOWS
    WIN32_FILE_ATTRIBUTE_DATA attributes;
    if (GetFileAttributesEx(m_fullPath.toWideCharPointer(), GetFileExInfoStandard, &attributes))
        return (((int64) attributes.nFileSizeHigh) << 32) | attributes.nFileSizeLow;
#else
    struct stat st;
    if (stat(m_fullPath.toUTF8(), &st) == 0) return st.st_size;
#endif

    return 0;
}

std::string File::parseAbsolutePath(const std::string& p)
{
    if (p.isEmpty()) return {};

#if GLUE_WINDOWS
    // Windows..
    auto path = normaliseSeparators(removeEllipsis(p.replaceCharacter('/', '\\')));

    if (path.startsWithChar(getSeparatorChar()))
    {
        if (path[1] != getSeparatorChar())
        {
            /*  When you supply a raw string to the File object constructor, it must be an absolute
               path. If you're trying to parse a string that may be either a relative path or an
               absolute path, you MUST provide a context against which the partial path can be
               evaluated - you can do this by simply using File::getChildFile() instead of the File
               constructor. E.g. saying "File::getCurrentWorkingDirectory().getChildFile
               (myUnknownPath)" would return an absolute path if that's what was supplied, or would
               evaluate a partial path relative to the CWD.
            */
            jassertfalse;

            path = File::getCurrentWorkingDirectory().getFullPathName().substring(0, 2) + path;
        }
    }
    else if (!path.containsChar(':'))
    {
        /*  When you supply a raw string to the File object constructor, it must be an absolute
           path. If you're trying to parse a string that may be either a relative path or an
           absolute path, you MUST provide a context against which the partial path can be evaluated
           - you can do this by simply using File::getChildFile() instead of the File constructor.
           E.g. saying "File::getCurrentWorkingDirectory().getChildFile (myUnknownPath)" would
           return an absolute path if that's what was supplied, or would evaluate a partial path
           relative to the CWD.
        */
        jassertfalse;

        return File::getCurrentWorkingDirectory().getChildFile(path).getFullPathName();
    }
#else
    // Mac or Linux..

    // Yes, I know it's legal for a unix pathname to contain a backslash, but this assertion is here
    // to catch anyone who's trying to run code that was written on Windows with hard-coded path
    // names. If that's why you've ended up here, use File::getChildFile() to build your paths
    // instead.
    jassert((!p.containsChar('\\')) ||
            (p.indexOfChar('/') >= 0 && p.indexOfChar('/') < p.indexOfChar('\\')));

    auto path = normaliseSeparators(removeEllipsis(p));

    if (path.startsWithChar('~'))
    {
        if (path[1] == getSeparatorChar() || path[1] == 0)
        {
            // expand a name of the form "~/abc"
            path = File::getSpecialLocation(File::userHomeDirectory).getFullPathName() +
                   path.substring(1);
        }
        else
        {
            // expand a name of type "~dave/abc"
            auto userName = path.substring(1).upToFirstOccurrenceOf("/", false, false);

            if (auto* pw = getpwnam(userName.toUTF8()))
                path = addTrailingSeparator(pw->pw_dir) +
                       path.fromFirstOccurrenceOf("/", false, false);
        }
    }
    else if (!path.startsWithChar(getSeparatorChar()))
    {
    #if GLUE_DEBUG || GLUE_LOG_ASSERTIONS
        if (!(path.startsWith("./") || path.startsWith("../")))
        {
            /*  When you supply a raw string to the File object constructor, it must be an absolute
               path. If you're trying to parse a string that may be either a relative path or an
               absolute path, you MUST provide a context against which the partial path can be
               evaluated - you can do this by simply using File::getChildFile() instead of the File
               constructor. E.g. saying "File::getCurrentWorkingDirectory().getChildFile
               (myUnknownPath)" would return an absolute path if that's what was supplied, or would
               evaluate a partial path relative to the CWD.
            */
            jassertfalse;

        #if GLUE_LOG_ASSERTIONS
            Logger::writeToLog("Illegal absolute path: " + path);
        #endif
        }
    #endif

        return File::getCurrentWorkingDirectory().getChildFile(path).getFullPathName();
    }
#endif

    while (path.endsWithChar(getSeparatorChar()) &&
           path != getSeparatorString()) // careful not to turn a single "/" into an empty string.
        path = path.dropLastCharacters(1);

    return path;
}

std::string File::addTrailingSeparator(const std::string& path)
{
    return path.endsWithChar(getSeparatorChar()) ? path : path + getSeparatorChar();
}

//==============================================================================
#if JUCE_LINUX || JUCE_BSD
    #define NAMES_ARE_CASE_SENSITIVE 1
#endif

static int compareFilenames(const std::string& name1, const std::string& name2) noexcept
{
#if NAMES_ARE_CASE_SENSITIVE
    return name1.compare(name2);
#else
    return name1.compareIgnoreCase(name2);
#endif
}

bool File::operator==(const File& other) const
{
    return compareFilenames(m_fullPath, other.m_fullPath) == 0;
}
bool File::operator!=(const File& other) const
{
    return compareFilenames(m_fullPath, other.m_fullPath) != 0;
}

bool File::moveFileTo(const File& newFile) const
{
    if (newFile.m_fullPath == m_fullPath) return true;

    if (!exists()) return false;

#if !NAMES_ARE_CASE_SENSITIVE
    if (*this != newFile)
#endif
        if (!newFile.deleteFile()) return false;

    return moveInternal(newFile);
}

bool File::copyFileTo(const File& newFile) const
{
    return (*this == newFile) || (exists() && newFile.deleteFile() && copyInternal(newFile));
}

bool File::replaceFileIn(const File& newFile) const
{
    if (newFile.m_fullPath == m_fullPath) return true;

    if (!newFile.exists()) return moveFileTo(newFile);

    if (!replaceInternal(newFile)) return false;

    deleteFile();
    return true;
}

//==============================================================================
std::string File::getPathUpToLastSlash() const
{
    auto lastSlash = m_fullPath.lastIndexOfChar(getSeparatorChar());

    if (lastSlash > 0) return m_fullPath.substring(0, lastSlash);

    if (lastSlash == 0) return getSeparatorString();

    return m_fullPath;
}

File File::getParentDirectory() const
{
    return createFileWithoutCheckingPath(getPathUpToLastSlash());
}

//==============================================================================
std::string File::getFileName() const
{
    return m_fullPath.substring(m_fullPath.lastIndexOfChar(getSeparatorChar()) + 1);
}

std::string File::getFileNameWithoutExtension() const
{
    auto lastSlash = m_fullPath.lastIndexOfChar(getSeparatorChar()) + 1;
    auto lastDot = m_fullPath.lastIndexOfChar('.');

    if (lastDot > lastSlash) return m_fullPath.substring(lastSlash, lastDot);

    return m_fullPath.substring(lastSlash);
}

//==============================================================================
bool File::isAbsolutePath(StringRef path)
{
    auto firstChar = *(path.text);

    return firstChar == getSeparatorChar()
#if GLUE_WINDOWS
           || (firstChar != 0 && path.text[1] == ':');
#else
           || firstChar == '~';
#endif
}

File File::getChildFile(StringRef relativePath) const
{
    auto r = relativePath.text;

    if (isAbsolutePath(r)) return File(std::string(r));

#if GLUE_WINDOWS
    if (r.indexOf((glue_wchar) '/') >= 0)
        return getChildFile(std::string(r).replaceCharacter('/', '\\'));
#endif

    auto path = m_fullPath;
    auto separatorChar = getSeparatorChar();

    while (*r == '.')
    {
        auto lastPos = r;
        auto secondChar = *++r;

        if (secondChar == '.') // remove "../"
        {
            auto thirdChar = *++r;

            if (thirdChar == separatorChar || thirdChar == 0)
            {
                auto lastSlash = path.lastIndexOfChar(separatorChar);

                if (lastSlash >= 0) path = path.substring(0, lastSlash);

                while (*r == separatorChar) // ignore duplicate slashes
                    ++r;
            }
            else
            {
                r = lastPos;
                break;
            }
        }
        else if (secondChar == separatorChar || secondChar == 0) // remove "./"
        {
            while (*r == separatorChar) // ignore duplicate slashes
                ++r;
        }
        else
        {
            r = lastPos;
            break;
        }
    }

    path = addTrailingSeparator(path);
    path.appendCharPointer(r);
    return File(path);
}

File File::getSiblingFile(StringRef fileName) const
{
    return getParentDirectory().getChildFile(fileName);
}

//==============================================================================
bool File::create() const
{
    if (exists()) return true;

    auto parentDir = getParentDirectory();

    if (parentDir == *this) return false;

    return parentDir.createDirectory();
}

bool File::createDirectory() const
{
    if (isDirectory()) return true;

    auto parentDir = getParentDirectory();

    if (parentDir == *this) return false;

    auto r = parentDir.createDirectory();

    if (r) r = createDirectoryInternal(m_fullPath.trimCharactersAtEnd(getSeparatorString()));

    return r;
}

bool File::deleteFile() const
{
#if GLUE_WINDOWS
    if (!exists()) return true;

    return isDirectory() ? RemoveDirectory(m_fullPath.toWideCharPointer()) != 0
                         : DeleteFile(m_fullPath.toWideCharPointer()) != 0;
#else
    if (!isSymbolicLink())
    {
        if (!exists()) return true;

        if (isDirectory()) return rmdir(m_fullPath.toUTF8()) == 0;
    }

    return remove(m_fullPath.toUTF8()) == 0;
#endif
}

//==============================================================================
File File::getNonexistentChildFile(const std::string& suggestedPrefix, const std::string& suffix,
                                   bool putNumbersInBrackets) const
{
    auto f = getChildFile(suggestedPrefix + suffix);

    if (f.exists())
    {
        int number = 1;
        auto prefix = suggestedPrefix;

        // remove any bracketed numbers that may already be on the end..
        if (prefix.trim().endsWithChar(')'))
        {
            putNumbersInBrackets = true;

            auto openBracks = prefix.lastIndexOfChar('(');
            auto closeBracks = prefix.lastIndexOfChar(')');

            if (openBracks > 0 && closeBracks > openBracks &&
                prefix.substring(openBracks + 1, closeBracks).containsOnly("0123456789"))
            {
                number = prefix.substring(openBracks + 1, closeBracks).getIntValue();
                prefix = prefix.substring(0, openBracks);
            }
        }

        do {
            auto newName = prefix;

            if (putNumbersInBrackets) { newName << '(' << ++number << ')'; }
            else
            {
                if (CharacterFunctions::isDigit(prefix.getLastCharacter()))
                    newName << '_'; // pad with an underscore if the name already ends in a digit

                newName << ++number;
            }

            f = getChildFile(newName + suffix);

        } while (f.exists());
    }

    return f;
}

File File::getNonexistentSibling(const bool putNumbersInBrackets) const
{
    if (!exists()) return *this;

    return getParentDirectory().getNonexistentChildFile(getFileNameWithoutExtension(),
                                                        getFileExtension(), putNumbersInBrackets);
}

//==============================================================================
std::string File::getFileExtension() const
{
    auto indexOfDot = m_fullPath.lastIndexOfChar('.');

    if (indexOfDot > m_fullPath.lastIndexOfChar(getSeparatorChar()))
        return m_fullPath.substring(indexOfDot);

    return {};
}

bool File::hasFileExtension(StringRef possibleSuffix) const
{
    if (possibleSuffix.isEmpty())
        return m_fullPath.lastIndexOfChar('.') <= m_fullPath.lastIndexOfChar(getSeparatorChar());

    auto semicolon = possibleSuffix.text.indexOf((glue_wchar) ';');

    if (semicolon >= 0)
        return hasFileExtension(std::string(possibleSuffix.text).substring(0, semicolon).trimEnd()) ||
               hasFileExtension((possibleSuffix.text + (semicolon + 1)).findEndOfWhitespace());

    if (m_fullPath.endsWithIgnoreCase(possibleSuffix))
    {
        if (possibleSuffix.text[0] == '.') return true;

        auto dotPos = m_fullPath.length() - possibleSuffix.length() - 1;

        if (dotPos >= 0) return m_fullPath[dotPos] == '.';
    }

    return false;
}

File File::setFileExtension(StringRef newExtension) const
{
    if (m_fullPath.isEmpty()) return {};

    auto filePart = getFileName();

    auto lastDot = filePart.lastIndexOfChar('.');

    if (lastDot >= 0) filePart = filePart.substring(0, lastDot);

    if (newExtension.isNotEmpty() && newExtension.text[0] != '.') filePart << '.';

    return getSiblingFile(filePart + newExtension);
}

} // namespace glue
