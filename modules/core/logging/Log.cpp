//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "logging/Log.h"

using std::map;
using std::shared_ptr;
using std::string;

namespace glue
{

static LogLevel currentLogLevel = GLUE_LOG_INFO;

bool Log::bAutoSpace = false;
string& Log::getPadding()
{
    static string* padding = new string;
    return *padding;
}

static map<string, LogLevel>& getModules()
{
    static map<string, LogLevel>* modules = new map<string, LogLevel>;
    return *modules;
}

static void noopDeleter(BaseLoggerChannel*) {}

shared_ptr<BaseLoggerChannel>& Log::channel()
{
#if GLUE_ANDROID
    static shared_ptr<BaseLoggerChannel> channel = shared_ptr<AndroidLogChannel>(
        new AndroidLogChannel, std::function<void(BaseLoggerChannel*)>(noopDeleter));
#elif GLUE_WINDOWS
    static shared_ptr<BaseLoggerChannel> channel =
        IsDebuggerPresent()
            ? shared_ptr<BaseLoggerChannel>(new DebugViewLoggerChannel,
                                            std::function<void(BaseLoggerChannel*)>(noopDeleter))
            : shared_ptr<BaseLoggerChannel>(new ConsoleLoggerChannel,
                                            std::function<void(BaseLoggerChannel*)>(noopDeleter));
#else
    static shared_ptr<BaseLoggerChannel> channel = shared_ptr<ConsoleLoggerChannel>(
        new ConsoleLoggerChannel, std::function<void(BaseLoggerChannel*)>(noopDeleter));
#endif

    return channel;
}

//--------------------------------------------------
void SetLogLevel(LogLevel level) { currentLogLevel = level; }

//--------------------------------------------------
void SetLogLevel(string module, LogLevel level) { getModules()[module] = level; }

//--------------------------------------------------
LogLevel GetLogLevel() { return currentLogLevel; }

//--------------------------------------------------
LogLevel GetLogLevel(string module)
{
    if (getModules().find(module) == getModules().end()) { return currentLogLevel; }
    else
    {
        return getModules()[module];
    }
}

//--------------------------------------------------
void LogToFile(const std::string& path, bool append)
{
    Log::setChannel(std::make_shared<FileLoggerChannel>(path, append));
}

//--------------------------------------------------
void LogToConsole()
{
    Log::setChannel(shared_ptr<ConsoleLoggerChannel>(
        new ConsoleLoggerChannel, std::function<void(BaseLoggerChannel*)>(noopDeleter)));
}

#if GLUE_WINDOWS
void LogToDebugView()
{
    Log::setChannel(shared_ptr<DebugViewLoggerChannel>(
        new DebugViewLoggerChannel, std::function<void(BaseLoggerChannel*)>(noopDeleter)));
}
#endif

//--------------------------------------------------
Log::Log()
{
    level = GLUE_LOG_INFO;
    module = "";
    bPrinted = false;
}

//--------------------------------------------------
Log::Log(LogLevel _level)
{
    level = _level;
    module = "";
    bPrinted = false;
}

//--------------------------------------------------
Log::Log(LogLevel level, const string& message)
{
    _log(level, "", message);
    bPrinted = true;
}

//--------------------------------------------------
void Log::setAutoSpace(bool autoSpace)
{
    bAutoSpace = autoSpace;
    if (bAutoSpace) { Log::getPadding() = " "; }
    else
    {
        Log::getPadding() = "";
    }
}

//-------------------------------------------------------
Log::~Log()
{
    // don't log if we printed in the constructor already
    if (!bPrinted) { _log(level, module, message.str()); }
}

bool Log::checkLog(LogLevel level, const string& module)
{
    if (getModules().find(module) == getModules().end())
    {
        if (level >= currentLogLevel) return true;
    }
    else
    {
        if (level >= getModules()[module]) return true;
    }
    return false;
}

//-------------------------------------------------------
void Log::_log(LogLevel level, const string& module, const string& message)
{
    if (checkLog(level, module)) { channel()->log(level, module, message); }
}

//--------------------------------------------------
LogDebug::LogDebug(const string& _module)
{
    level = GLUE_LOG_DEBUG;
    module = _module;
    bPrinted = false;
}

LogDebug::LogDebug(const string& _module, const string& _message)
{
    _log(GLUE_LOG_DEBUG, _module, _message);
    bPrinted = true;
}

//--------------------------------------------------
LogInfo::LogInfo(const string& _module)
{
    level = GLUE_LOG_INFO;
    module = _module;
    bPrinted = false;
}

LogInfo::LogInfo(const string& _module, const string& _message)
{
    _log(GLUE_LOG_INFO, _module, _message);
    bPrinted = true;
}

//--------------------------------------------------
LogWarn::LogWarn(const string& _module)
{
    level = GLUE_LOG_WARN;
    module = _module;
    bPrinted = false;
}

LogWarn::LogWarn(const string& _module, const string& _message)
{
    _log(GLUE_LOG_WARN, _module, _message);
    bPrinted = true;
}

//--------------------------------------------------
LogError::LogError(const string& _module)
{
    level = GLUE_LOG_ERROR;
    module = _module;
    bPrinted = false;
}

LogError::LogError(const string& _module, const string& _message)
{
    _log(GLUE_LOG_ERROR, _module, _message);
    bPrinted = true;
}

//--------------------------------------------------
LogFatal::LogFatal(const string& _module)
{
    level = GLUE_LOG_FATAL;
    module = _module;
    bPrinted = false;
}

LogFatal::LogFatal(const string& _module, const string& _message)
{
    _log(GLUE_LOG_FATAL, _module, _message);
    bPrinted = true;
}

//--------------------------------------------------
void Log::setChannel(shared_ptr<BaseLoggerChannel> _channel) { channel() = _channel; }

void SetLoggerChannel(shared_ptr<BaseLoggerChannel> loggerChannel)
{
    Log::setChannel(loggerChannel);
}

shared_ptr<BaseLoggerChannel> Log::getChannel() { return channel(); }

shared_ptr<BaseLoggerChannel> GetLoggerChannel() { return Log::getChannel(); }

string GetLogLevelName(LogLevel level, bool pad)
{
    switch (level)
    {
    case GLUE_LOG_DEBUG:
        return "verbose";
    case GLUE_LOG_INFO:
        return pad ? "notice " : "notice";
    case GLUE_LOG_WARN:
        return "warning";
    case GLUE_LOG_ERROR:
        return pad ? " error " : "error";
    case GLUE_LOG_FATAL:
        return pad ? " fatal " : "fatal";
    case GLUE_LOG_SILENT:
        return pad ? "silent " : "silent";
    default:
        return "";
    }
}

//--------------------------------------------------
void ConsoleLoggerChannel::log(LogLevel level, const string& module, const string& message)
{
    // print to cerr for GLUE_LOG_ERROR and GLUE_LOG_FATAL, everything else to cout
    std::ostream& out = level < GLUE_LOG_ERROR ? std::cout : std::cerr;
    out << "[" << GetLogLevelName(level, true) << "] ";
    // only print the module name if it's not ""
    if (module != "") { out << module << ": "; }
    out << message << std::endl;
}

#if GLUE_ANDROID
void AndroidLogChannel::log(LogLevel level, const string& module, const string& msg)
{
    android_LogPriority androidPrio;
    switch (level)
    {
    case GLUE_LOG_DEBUG:
        androidPrio = ANDROID_LOG_DEBUG;
        break;
    case GLUE_LOG_INFO:
        androidPrio = ANDROID_LOG_INFO;
        break;
    case GLUE_LOG_WARN:
        androidPrio = ANDROID_LOG_WARN;
        break;
    case GLUE_LOG_ERROR:
        androidPrio = ANDROID_LOG_ERROR;
        break;
    case GLUE_LOG_FATAL:
        androidPrio = ANDROID_LOG_FATAL;
        break;
    default:
        androidPrio = ANDROID_LOG_INFO;
        break;
    }
    __android_log_print(androidPrio, module.c_str(), msg.c_str());
}
#endif

#if GLUE_WINDOWS
void DebugViewLoggerChannel::log(LogLevel level, const string& module, const string& message)
{
    // print to cerr for GLUE_LOG_ERROR and GLUE_LOG_FATAL, everything else to cout
    std::stringstream out;
    out << "[" << GetLogLevelName(level, true) << "] ";
    // only print the module name if it's not ""
    if (module != "") { out << module << ": "; }
    out << message << std::endl;
    OutputDebugStringA(out.str().c_str());
}
#endif

//--------------------------------------------------
FileLoggerChannel::FileLoggerChannel() {}

FileLoggerChannel::FileLoggerChannel(const std::string& path, bool append)
{
    setFile(path, append);
}

FileLoggerChannel::~FileLoggerChannel() { close(); }

void FileLoggerChannel::close() { m_file.close(); }

void FileLoggerChannel::setFile(const std::string& path, bool append)
{
    m_file.open(path, append ? ofFile::Append : ofFile::WriteOnly);
    m_file << std::endl;
    m_file << std::endl;
    m_file << "--------------------------------------- " << ofGetTimestampString() << std::endl;
}

void FileLoggerChannel::log(LogLevel level, const string& module, const string& message)
{
    m_file << "[" << GetLogLevelName(level, true) << "] ";
    if (module != "") { m_file << module << ": "; }
    m_file << message << std::endl;
}

} // namespace glue
