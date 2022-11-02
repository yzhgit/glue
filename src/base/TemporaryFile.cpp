//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/TemporaryFile.h"

#include "glue/base/Thread.h"

#include <stdlib.h>
#include <stdio.h>
#include <fileapi.h>

namespace glue
{

static File createTempFile(const File& parentDirectory, String name, const String& suffix,
                           int optionFlags)
{
    if ((optionFlags & TemporaryFile::useHiddenFile) != 0) name = "." + name;

    return parentDirectory.getNonexistentChildFile(
        name, suffix, (optionFlags & TemporaryFile::putNumbersInBrackets) != 0);
}

TemporaryFile::TemporaryFile(const String& suffix, const int optionFlags)
    : m_temporaryFile(createTempFile(File::getSpecialLocation(File::tempDirectory),
                                   "temp_" + String::toHexString(lockedRandom.nextInt()), suffix,
                                   optionFlags))
    , m_targetFile()
{}

TemporaryFile::TemporaryFile(const File& target, const int optionFlags)
    : m_temporaryFile(createTempFile(target.getParentDirectory(),
                                   target.getFileNameWithoutExtension() + "_temp" +
                                       String::toHexString(lockedRandom.nextInt()),
                                   target.getFileExtension(), optionFlags))
    , m_targetFile(target)
{
    // If you use this constructor, you need to give it a valid target file!
    GLUE_ASSERT(m_targetFile != File());
}

TemporaryFile::TemporaryFile(const File& target, const File& temporary)
    : m_temporaryFile(temporary), m_targetFile(target)
{}

TemporaryFile::~TemporaryFile()
{
    if (!deleteTemporaryFile())
    {
        /* Failed to delete our temporary file! The most likely reason for this would be
           that you've not closed an output stream that was being used to write to file.

           If you find that something beyond your control is changing permissions on
           your temporary files and preventing them from being deleted, you may want to
           call TemporaryFile::deleteTemporaryFile() to detect those error cases and
           handle them appropriately.
        */
        jassertfalse;
    }
}

//==============================================================================
bool TemporaryFile::overwriteTargetFileWithTemporary() const
{
    // This method only works if you created this object with the constructor
    // that takes a target file!
    GLUE_ASSERT(m_targetFile != File());

    if (m_temporaryFile.exists())
    {
        // Have a few attempts at overwriting the file before giving up..
        for (int i = 5; --i >= 0;)
        {
            if (m_temporaryFile.replaceFileIn(m_targetFile)) return true;

            Thread::sleep(100);
        }
    }
    else
    {
        // There's no temporary file to use. If your write failed, you should
        // probably check, and not bother calling this method.
        jassertfalse;
    }

    return false;
}

bool TemporaryFile::deleteTemporaryFile() const
{
    // Have a few attempts at deleting the file before giving up..
    for (int i = 5; --i >= 0;)
    {
        if (m_temporaryFile.deleteFile())
            return true;

        Thread::sleep(50);
    }

    return false;
}

} // namespace glue
