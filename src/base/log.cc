//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/log.h"

using std::map;
using std::shared_ptr;
using std::string;

GLUE_START_NAMESPACE

static LogLevel currentLogLevel = GLUE_LOG_INFO;

bool Log::m_bAutoSpace = false;

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

static void noopDeleter(BaseLoggerChannel*)
{}

shared_ptr<BaseLoggerChannel>& Log::channel()
{
#if defined(GLUE_OS_ANDROID)
    static shared_ptr<BaseLoggerChannel> channel = shared_ptr<AndroidLogChannel>(
        new AndroidLogChannel, std::function<void(BaseLoggerChannel*)>(noopDeleter));
#elif defined(GLUE_OS_WINDOWS)
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

void SetLogLevel(LogLevel level)
{
    currentLogLevel = level;
}

void SetLogLevel(string module, LogLevel level)
{
    getModules()[module] = level;
}

LogLevel GetLogLevel()
{
    return currentLogLevel;
}

LogLevel GetLogLevel(string module)
{
    if (getModules().find(module) == getModules().end()) { return currentLogLevel; }
    else
    {
        return getModules()[module];
    }
}

void LogToFile(const std::string& path, bool append)
{
    Log::setChannel(std::make_shared<FileLoggerChannel>(path, append));
}

void LogToConsole()
{
    Log::setChannel(shared_ptr<ConsoleLoggerChannel>(
        new ConsoleLoggerChannel, std::function<void(BaseLoggerChannel*)>(noopDeleter)));
}

#if defined(GLUE_OS_WINDOWS)
void LogToDebugView()
{
    Log::setChannel(shared_ptr<DebugViewLoggerChannel>(
        new DebugViewLoggerChannel, std::function<void(BaseLoggerChannel*)>(noopDeleter)));
}
#endif

Log::Log()
{
    m_level = GLUE_LOG_INFO;
    m_module = "";
    m_bPrinted = false;
}

Log::Log(LogLevel _level)
{
    m_level = _level;
    m_module = "";
    m_bPrinted = false;
}

Log::Log(LogLevel level, const string& message)
{
    _log(level, "", message);
    m_bPrinted = true;
}

void Log::setAutoSpace(bool autoSpace)
{
    m_bAutoSpace = autoSpace;
    if (m_bAutoSpace) { Log::getPadding() = " "; }
    else
    {
        Log::getPadding() = "";
    }
}

Log::~Log()
{
    // don't log if we printed in the constructor already
    if (!m_bPrinted) { _log(m_level, m_module, m_message.str()); }
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

void Log::_log(LogLevel level, const string& module, const string& message)
{
    if (checkLog(level, module)) { channel()->log(level, module, message); }
}

LogDebug::LogDebug(const string& _module)
{
    m_level = GLUE_LOG_DEBUG;
    m_module = _module;
    m_bPrinted = false;
}

LogDebug::LogDebug(const string& _module, const string& _message)
{
    _log(GLUE_LOG_DEBUG, _module, _message);
    m_bPrinted = true;
}

LogInfo::LogInfo(const string& _module)
{
    m_level = GLUE_LOG_INFO;
    m_module = _module;
    m_bPrinted = false;
}

LogInfo::LogInfo(const string& _module, const string& _message)
{
    _log(GLUE_LOG_INFO, _module, _message);
    m_bPrinted = true;
}

LogWarn::LogWarn(const string& _module)
{
    m_level = GLUE_LOG_WARN;
    m_module = _module;
    m_bPrinted = false;
}

LogWarn::LogWarn(const string& _module, const string& _message)
{
    _log(GLUE_LOG_WARN, _module, _message);
    m_bPrinted = true;
}

LogError::LogError(const string& _module)
{
    m_level = GLUE_LOG_ERROR;
    m_module = _module;
    m_bPrinted = false;
}

LogError::LogError(const string& _module, const string& _message)
{
    _log(GLUE_LOG_ERROR, _module, _message);
    m_bPrinted = true;
}

LogFatal::LogFatal(const string& _module)
{
    m_level = GLUE_LOG_FATAL;
    m_module = _module;
    m_bPrinted = false;
}

LogFatal::LogFatal(const string& _module, const string& _message)
{
    _log(GLUE_LOG_FATAL, _module, _message);
    m_bPrinted = true;
}

void Log::setChannel(shared_ptr<BaseLoggerChannel> _channel)
{
    channel() = _channel;
}

void SetLoggerChannel(shared_ptr<BaseLoggerChannel> loggerChannel)
{
    Log::setChannel(loggerChannel);
}

shared_ptr<BaseLoggerChannel> Log::getChannel()
{
    return channel();
}

shared_ptr<BaseLoggerChannel> GetLoggerChannel()
{
    return Log::getChannel();
}

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

void ConsoleLoggerChannel::log(LogLevel level, const string& module, const string& message)
{
    std::ostream& out = level < GLUE_LOG_ERROR ? std::cout : std::cerr;
    out << "[" << GetLogLevelName(level, true) << "] ";
    if (module != "") { out << module << ": "; }
    out << message << std::endl;
}

#if defined(GLUE_OS_ANDROID)
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

#if defined(GLUE_OS_WINDOWS)
void DebugViewLoggerChannel::log(LogLevel level, const string& module, const string& message)
{
    std::stringstream out;
    out << "[" << GetLogLevelName(level, true) << "] ";
    if (module != "") { out << module << ": "; }
    out << message << std::endl;
    OutputDebugStringA(out.str().c_str());
}
#endif

FileLoggerChannel::FileLoggerChannel()
{}

FileLoggerChannel::FileLoggerChannel(const std::string& path, bool append)
{
    setFile(path, append);
}

FileLoggerChannel::~FileLoggerChannel()
{
    close();
}

void FileLoggerChannel::close()
{
    m_fileStream.close();
}

void FileLoggerChannel::setFile(const std::string& path, bool append)
{
    m_fileStream.open(path, append ? std::ios::app : std::ios::trunc);
}

void FileLoggerChannel::log(LogLevel level, const string& module, const string& message)
{
    m_fileStream << "[" << GetLogLevelName(level, true) << "] ";
    if (module != "") { m_fileStream << module << ": "; }
    m_fileStream << message << std::endl;
}

GLUE_END_NAMESPACE
