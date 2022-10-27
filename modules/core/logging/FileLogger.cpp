//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "logging/FileLogger.h"

#include "time/Time.h"
#include "files/TemporaryFile.h"

namespace glue
{

FileLogger::FileLogger(const File& file, const String& welcomeMessage,
                       const int64 maxInitialFileSizeBytes)
    : logFile(file)
{
    if (maxInitialFileSizeBytes >= 0) trimFileSize(logFile, maxInitialFileSizeBytes);

    if (!file.exists()) file.create(); // (to create the parent directories)

    String welcome;
    welcome << newLine << "**********************************************************" << newLine
            << welcomeMessage << newLine
            << "Log started: " << Time::getCurrentTime().toString(true, true) << newLine;

    FileLogger::logMessage(welcome);
}

FileLogger::~FileLogger() {}

//==============================================================================
void FileLogger::logMessage(const String& message)
{
    const ScopedLock sl(logLock);
    DBG(message);
    FileOutputStream out(logFile, 256);
    out << message << newLine;
}

void FileLogger::trimFileSize(const File& file, int64 maxFileSizeBytes)
{
    if (maxFileSizeBytes <= 0) { file.deleteFile(); }
    else
    {
        const int64 fileSize = file.getSize();

        if (fileSize > maxFileSizeBytes)
        {
            TemporaryFile tempFile(file);

            {
                FileOutputStream out(tempFile.getFile());
                FileInputStream in(file);

                if (!(out.openedOk() && in.openedOk())) return;

                in.setPosition(fileSize - maxFileSizeBytes);

                for (;;)
                {
                    const char c = in.readByte();
                    if (c == 0) return;

                    if (c == '\n' || c == '\r')
                    {
                        out << c;
                        break;
                    }
                }

                out.writeFromInputStream(in, -1);
            }

            tempFile.overwriteTargetFileWithTemporary();
        }
    }
}

//==============================================================================
File FileLogger::getSystemLogFileFolder()
{
    return File::getSpecialLocation(File::userApplicationDataDirectory);
}

FileLogger* FileLogger::createDefaultAppLogger(const String& logFileSubDirectoryName,
                                               const String& logFileName,
                                               const String& welcomeMessage,
                                               const int64 maxInitialFileSizeBytes)
{
    return new FileLogger(
        getSystemLogFileFolder().getChildFile(logFileSubDirectoryName).getChildFile(logFileName),
        welcomeMessage, maxInitialFileSizeBytes);
}

FileLogger* FileLogger::createDateStampedLogger(const String& logFileSubDirectoryName,
                                                const String& logFileNameRoot,
                                                const String& logFileNameSuffix,
                                                const String& welcomeMessage)
{
    return new FileLogger(
        getSystemLogFileFolder()
            .getChildFile(logFileSubDirectoryName)
            .getChildFile(logFileNameRoot + Time::getCurrentTime().formatted("%Y-%m-%d_%H-%M-%S"))
            .withFileExtension(logFileNameSuffix)
            .getNonexistentSibling(),
        welcomeMessage, 0);
}

} // namespace glue
