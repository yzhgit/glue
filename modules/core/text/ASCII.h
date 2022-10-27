//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "text/CharacterFunctions.h"

namespace glue
{

//==============================================================================
/**
    Wraps a pointer to a null-terminated ASCII character string, and provides
    various methods to operate on the data.

    A valid ASCII string is assumed to not contain any characters above 127.

    @see UTF8, UTF16, UTF32

    @tags{Core}
*/
class ASCII final
{
public:
    using CharType = char;

    inline explicit ASCII(const CharType* rawPointer) noexcept
        : data(const_cast<CharType*>(rawPointer))
    {}

    inline ASCII(const ASCII& other) = default;

    inline ASCII operator=(const ASCII other) noexcept
    {
        data = other.data;
        return *this;
    }

    inline ASCII operator=(const CharType* text) noexcept
    {
        data = const_cast<CharType*>(text);
        return *this;
    }

    /** This is a pointer comparison, it doesn't compare the actual text. */
    inline bool operator==(ASCII other) const noexcept { return data == other.data; }
    inline bool operator!=(ASCII other) const noexcept { return data != other.data; }
    inline bool operator<=(ASCII other) const noexcept { return data <= other.data; }
    inline bool operator<(ASCII other) const noexcept { return data < other.data; }
    inline bool operator>=(ASCII other) const noexcept { return data >= other.data; }
    inline bool operator>(ASCII other) const noexcept { return data > other.data; }

    /** Returns the address that this pointer is pointing to. */
    inline CharType* getAddress() const noexcept { return data; }

    /** Returns the address that this pointer is pointing to. */
    inline operator const CharType*() const noexcept { return data; }

    /** Returns true if this pointer is pointing to a null character. */
    inline bool isEmpty() const noexcept { return *data == 0; }

    /** Returns true if this pointer is not pointing to a null character. */
    inline bool isNotEmpty() const noexcept { return *data != 0; }

    /** Returns the unicode character that this pointer is pointing to. */
    inline glue_wchar operator*() const noexcept { return (glue_wchar) (uint8) *data; }

    /** Moves this pointer along to the next character in the string. */
    inline ASCII operator++() noexcept
    {
        ++data;
        return *this;
    }

    /** Moves this pointer to the previous character in the string. */
    inline ASCII operator--() noexcept
    {
        --data;
        return *this;
    }

    /** Returns the character that this pointer is currently pointing to, and then
        advances the pointer to point to the next character. */
    inline glue_wchar getAndAdvance() noexcept { return (glue_wchar) (uint8) *data++; }

    /** Moves this pointer along to the next character in the string. */
    ASCII operator++(int) noexcept
    {
        auto temp(*this);
        ++data;
        return temp;
    }

    /** Moves this pointer forwards by the specified number of characters. */
    inline void operator+=(const int numToSkip) noexcept { data += numToSkip; }

    inline void operator-=(const int numToSkip) noexcept { data -= numToSkip; }

    /** Returns the character at a given character index from the start of the string. */
    inline glue_wchar operator[](const int characterIndex) const noexcept
    {
        return (glue_wchar) (uint8) data[characterIndex];
    }

    /** Returns a pointer which is moved forwards from this one by the specified number of
     * characters. */
    ASCII operator+(const int numToSkip) const noexcept { return ASCII(data + numToSkip); }

    /** Returns a pointer which is moved backwards from this one by the specified number of
     * characters. */
    ASCII operator-(const int numToSkip) const noexcept { return ASCII(data - numToSkip); }

    /** Writes a unicode character to this string, and advances this pointer to point to the next
     * position. */
    inline void write(const glue_wchar charToWrite) noexcept { *data++ = (char) charToWrite; }

    inline void replaceChar(const glue_wchar newChar) noexcept { *data = (char) newChar; }

    /** Writes a null character to this string (leaving the pointer's position unchanged). */
    inline void writeNull() const noexcept { *data = 0; }

    /** Returns the number of characters in this string. */
    size_t length() const noexcept { return (size_t) strlen(data); }

    /** Returns the number of characters in this string, or the given value, whichever is lower. */
    size_t lengthUpTo(const size_t maxCharsToCount) const noexcept
    {
        return CharacterFunctions::lengthUpTo(*this, maxCharsToCount);
    }

    /** Returns the number of characters in this string, or up to the given end pointer, whichever
     * is lower. */
    size_t lengthUpTo(const ASCII end) const noexcept
    {
        return CharacterFunctions::lengthUpTo(*this, end);
    }

    /** Returns the number of bytes that are used to represent this string.
        This includes the terminating null character.
    */
    size_t sizeInBytes() const noexcept { return length() + 1; }

    /** Returns the number of bytes that would be needed to represent the given
        unicode character in this encoding format.
    */
    static size_t getBytesRequiredFor(const glue_wchar) noexcept { return 1; }

    /** Returns the number of bytes that would be needed to represent the given
        string in this encoding format.
        The value returned does NOT include the terminating null character.
    */
    template <class CharPointer>
    static size_t getBytesRequiredFor(const CharPointer text) noexcept
    {
        return text.length();
    }

    /** Returns a pointer to the null character that terminates this string. */
    ASCII findTerminatingNull() const noexcept { return ASCII(data + length()); }

    /** Copies a source string to this pointer, advancing this pointer as it goes. */
    template <typename CharPointer>
    void writeAll(const CharPointer src) noexcept
    {
        CharacterFunctions::copyAll(*this, src);
    }

    /** Copies a source string to this pointer, advancing this pointer as it goes.
        The maxDestBytes parameter specifies the maximum number of bytes that can be written
        to the destination buffer before stopping.
    */
    template <typename CharPointer>
    size_t writeWithDestByteLimit(const CharPointer src, const size_t maxDestBytes) noexcept
    {
        return CharacterFunctions::copyWithDestByteLimit(*this, src, maxDestBytes);
    }

    /** Copies a source string to this pointer, advancing this pointer as it goes.
        The maxChars parameter specifies the maximum number of characters that can be
        written to the destination buffer before stopping (including the terminating null).
    */
    template <typename CharPointer>
    void writeWithCharLimit(const CharPointer src, const int maxChars) noexcept
    {
        CharacterFunctions::copyWithCharLimit(*this, src, maxChars);
    }

    /** Compares this string with another one. */
    template <typename CharPointer>
    int compare(const CharPointer other) const noexcept
    {
        return CharacterFunctions::compare(*this, other);
    }

    /** Compares this string with another one. */
    int compare(const ASCII other) const noexcept { return strcmp(data, other.data); }

    /** Compares this string with another one, up to a specified number of characters. */
    template <typename CharPointer>
    int compareUpTo(const CharPointer other, const int maxChars) const noexcept
    {
        return CharacterFunctions::compareUpTo(*this, other, maxChars);
    }

    /** Compares this string with another one, up to a specified number of characters. */
    int compareUpTo(const ASCII other, const int maxChars) const noexcept
    {
        return strncmp(data, other.data, (size_t) maxChars);
    }

    /** Compares this string with another one. */
    template <typename CharPointer>
    int compareIgnoreCase(const CharPointer other) const
    {
        return CharacterFunctions::compareIgnoreCase(*this, other);
    }

    int compareIgnoreCase(const ASCII other) const
    {
#if GLUE_MINGW || (GLUE_WINDOWS && GLUE_CLANG)
        return CharacterFunctions::compareIgnoreCase(*this, other);
#elif GLUE_WINDOWS
        return stricmp(data, other.data);
#else
        return strcasecmp(data, other.data);
#endif
    }

    /** Compares this string with another one, up to a specified number of characters. */
    template <typename CharPointer>
    int compareIgnoreCaseUpTo(const CharPointer other, const int maxChars) const noexcept
    {
        return CharacterFunctions::compareIgnoreCaseUpTo(*this, other, maxChars);
    }

    /** Returns the character index of a substring, or -1 if it isn't found. */
    template <typename CharPointer>
    int indexOf(const CharPointer stringToFind) const noexcept
    {
        return CharacterFunctions::indexOf(*this, stringToFind);
    }

    /** Returns the character index of a unicode character, or -1 if it isn't found. */
    int indexOf(const glue_wchar charToFind) const noexcept
    {
        int i = 0;

        while (data[i] != 0)
        {
            if (data[i] == (char) charToFind) return i;

            ++i;
        }

        return -1;
    }

    /** Returns the character index of a unicode character, or -1 if it isn't found. */
    int indexOf(const glue_wchar charToFind, const bool ignoreCase) const noexcept
    {
        return ignoreCase ? CharacterFunctions::indexOfCharIgnoreCase(*this, charToFind)
                          : CharacterFunctions::indexOfChar(*this, charToFind);
    }

    /** Returns true if the first character of this string is whitespace. */
    bool isWhitespace() const { return CharacterFunctions::isWhitespace(*data) != 0; }
    /** Returns true if the first character of this string is a digit. */
    bool isDigit() const { return CharacterFunctions::isDigit(*data) != 0; }
    /** Returns true if the first character of this string is a letter. */
    bool isLetter() const { return CharacterFunctions::isLetter(*data) != 0; }
    /** Returns true if the first character of this string is a letter or digit. */
    bool isLetterOrDigit() const { return CharacterFunctions::isLetterOrDigit(*data) != 0; }
    /** Returns true if the first character of this string is upper-case. */
    bool isUpperCase() const
    {
        return CharacterFunctions::isUpperCase((glue_wchar) (uint8) *data) != 0;
    }
    /** Returns true if the first character of this string is lower-case. */
    bool isLowerCase() const
    {
        return CharacterFunctions::isLowerCase((glue_wchar) (uint8) *data) != 0;
    }

    /** Returns an upper-case version of the first character of this string. */
    glue_wchar toUpperCase() const noexcept
    {
        return CharacterFunctions::toUpperCase((glue_wchar) (uint8) *data);
    }
    /** Returns a lower-case version of the first character of this string. */
    glue_wchar toLowerCase() const noexcept
    {
        return CharacterFunctions::toLowerCase((glue_wchar) (uint8) *data);
    }

    /** Parses this string as a 32-bit integer. */
    int getIntValue32() const noexcept { return atoi(data); }

    /** Parses this string as a 64-bit integer. */
    int64 getIntValue64() const noexcept
    {
#if GLUE_LINUX || GLUE_BSD || GLUE_ANDROID || GLUE_MINGW
        return atoll(data);
#elif GLUE_WINDOWS
        return _atoi64(data);
#else
        return CharacterFunctions::getIntValue<int64, ASCII>(*this);
#endif
    }

    /** Parses this string as a floating point double. */
    double getDoubleValue() const noexcept { return CharacterFunctions::getDoubleValue(*this); }

    /** Returns the first non-whitespace character in the string. */
    ASCII findEndOfWhitespace() const noexcept
    {
        return CharacterFunctions::findEndOfWhitespace(*this);
    }

    /** Move this pointer to the first non-whitespace character in the string. */
    void incrementToEndOfWhitespace() noexcept
    {
        CharacterFunctions::incrementToEndOfWhitespace(*this);
    }

    /** Returns true if the given unicode character can be represented in this encoding. */
    static bool canRepresent(glue_wchar character) noexcept
    {
        return ((unsigned int) character) < (unsigned int) 128;
    }

    /** Returns true if this data contains a valid string in this encoding. */
    static bool isValidString(const CharType* dataToTest, int maxBytesToRead)
    {
        while (--maxBytesToRead >= 0)
        {
            if (((signed char) *dataToTest) <= 0) return *dataToTest == 0;

            ++dataToTest;
        }

        return true;
    }

private:
    CharType* data;
};

} // namespace glue
