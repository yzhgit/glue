//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "containers/Array.h"
#include "text/StringRef.h"

namespace glue
{

//==============================================================================
/**
    A StringPool holds a set of shared strings, which reduces storage overheads and improves
    comparison speed when dealing with many duplicate strings.

    When you add a string to a pool using getPooledString, it'll return a character
    array containing the same string. This array is owned by the pool, and the same array
    is returned every time a matching string is asked for. This means that it's trivial to
    compare two pooled strings for equality, as you can simply compare their pointers. It
    also cuts down on storage if you're using many copies of the same string.

    @tags{Core}
*/
class GLUE_API StringPool
{
public:
    //==============================================================================
    /** Creates an empty pool. */
    StringPool() noexcept;

    //==============================================================================
    /** Returns a pointer to a shared copy of the string that is passed in.
        The pool will always return the same String object when asked for a string that matches it.
    */
    String getPooledString(const String& original);

    /** Returns a pointer to a copy of the string that is passed in.
        The pool will always return the same String object when asked for a string that matches it.
    */
    String getPooledString(const char* original);

    /** Returns a pointer to a shared copy of the string that is passed in.
        The pool will always return the same String object when asked for a string that matches it.
    */
    String getPooledString(StringRef original);

    /** Returns a pointer to a copy of the string that is passed in.
        The pool will always return the same String object when asked for a string that matches it.
    */
    String getPooledString(String::CharPointerType start, String::CharPointerType end);

    //==============================================================================
    /** Scans the pool, and removes any strings that are unreferenced.
        You don't generally need to call this - it'll be called automatically when the pool grows
        large enough to warrant it.
    */
    void garbageCollect();

    /** Returns a shared global pool which is used for things like Identifiers, XML parsing. */
    static StringPool& getGlobalPool() noexcept;

private:
    Array<String> strings;
    std::mutex mtx;
    uint32 lastGarbageCollectionTime;

    void garbageCollectIfNeeded();

    GLUE_DECLARE_NON_COPYABLE(StringPool)
};

} // namespace glue
