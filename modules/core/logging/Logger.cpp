//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "logging/Logger.h"

#include "files/File.h"

namespace glue
{

Logger::Logger() {}

Logger::~Logger()
{
    // You're deleting this logger while it's still being used!
    // Always call Logger::setCurrentLogger (nullptr) before deleting the active logger.
    jassert(currentLogger != this);
}

Logger* Logger::currentLogger = nullptr;

void Logger::setCurrentLogger(Logger* const newLogger) noexcept { currentLogger = newLogger; }
Logger* Logger::getCurrentLogger() noexcept { return currentLogger; }

void Logger::writeToLog(const String& message)
{
    if (currentLogger != nullptr)
        currentLogger->logMessage(message);
    else
        outputDebugString(message);
}

#if GLUE_LOG_ASSERTIONS || GLUE_DEBUG
void GLUE_API GLUE_CALLTYPE logAssertion(const char* const filename, const int lineNum) noexcept
{
    String m("Assertion failure in ");
    m << File::createFileWithoutCheckingPath(UTF8(filename)).getFileName() << ':' << lineNum;

    #if GLUE_LOG_ASSERTIONS
    Logger::writeToLog(m);
    #else
    DBG(m);
    #endif
}
#endif

} // namespace glue
