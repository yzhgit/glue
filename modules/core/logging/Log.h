//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "files/File.h"

namespace glue
{

/// \file
/// Log provides an interface for writing text output from your app.
/// It's basically a more useful version of `std::cout` or `printf` where
/// the output can be filtered and written to the console a file, or even a
/// custom logging module.
///
/// Sometimes you want to be able to see when something has happened inside
/// the code, but don't need to draw something visually. Oftentimes it's
/// more then enough to print out the state of a few variables when debugging.
/// Other times you need to know if a crash happened while your app was
/// running somewhere, so you log messages and variables to a file you can
/// read after the program crashes.
///
/// ### Log Levels
/// You can set the logging level so only messages above a certain level
/// are shown. This is useful if you want see lots of messages when debugging,
/// but then set a higher level so only warnings and errors appear for users.
///
/// See SetLogLevel(LogLevel level) for more details.
///
/// ### Usage
/// There are 2 ways you can use Log:
///
/// #### Functional: as a function taking a message
///
/// ~~~~{.cpp}
/// // Send a single string message, setting the log level.
/// Log(GLUE_LOG_INFO, "the number is " + ofToString(10));
///
/// // The legacy printf style.
/// Log(GLUE_LOG_INFO, "the number is %d", 10);
/// ~~~~
///
/// #### Stream: as a stream using the << stream operator
///
/// ~~~~{.cpp}
/// // The stream style, setting the log level to GLUE_LOG_WARN.
/// Log(GLUE_LOG_WARN) << "the number is " << 10;
///
/// // This is the same as the last line, except it uses the default GLUE_LOG_INFO.
/// Log() << "the number is " << 10;
///
/// // There are also log level-specific stream objects, one for each level
/// // except GLUE_LOG_SILENT.
/// LogDebug() << "A verbose message."
/// LogInfo() << "A regular notice message.";
/// LogWarn() << "Uh oh, a warning!";
/// LogError() << "Oh no, an error occurred!";
/// LogFatal() << "Accckkk, a fatal error!!";
/// ~~~~
///
/// **Note**: The log level specific stream objects also take a string argument
/// for the "module". A module is a string that is added to the beginning of
/// the log line and can be used to separate logging messages by setting an
/// independent log level for **that module only**. This module-specific log
/// level has no effect on other modules.
///
/// See SetLogLevel(string module, LogLevel level) for more details.
///
/// Example of logging to a specific module:
/// ~~~~{.cpp}
/// // log to a module called "Hello"
/// LogWarn("Hello") << "A warning message.";
/// ~~~~
///
/// **Warning**: It is important to understand that the log level specific
/// stream objects take the module name as an argument and the log messages via
/// the << operator. Putting your message as a string argument inside the
/// parentheses uses that message as a *module* and so nothing will be printed:
///
/// ~~~~{.cpp}
/// // This prints a warning message.
/// LogWarn() << "A warning message.";
///
/// // !!! This does not print a message because the string "a warning print"
/// // is the module argument !!!
/// LogWarn("A warning print");
///
/// // This prints a warning message to the "Hello" module.
/// LogWarn("Hello") << "A warning message.";
/// ~~~~
///
/// ####Log Message Redirection
///
/// It's useful to be able to record log messages to a file or send them to a
/// custom destination.
///
/// For log redirection see
/// - LogToFile()
/// - LogToConsole()
/// - SetLoggerChannel()

/// \cond INTERNAL
/// printf annotations for automatic format checking in GCC.
#ifdef __GNUC__
    #define OF_PRINTF_ATTR(x, y) __attribute__((format(printf, x, y)))
#else
    #define OF_PRINTF_ATTR(x, y)
#endif
/// \endcond

//--------------------------------------------------
/// \name Global logging level
/// \{

/// \brief The supported logging levels. Default is `GLUE_LOG_INFO`.
enum LogLevel : short
{
    GLUE_LOG_DEBUG,
    GLUE_LOG_INFO,
    GLUE_LOG_WARN,
    GLUE_LOG_ERROR,
    GLUE_LOG_FATAL,
    GLUE_LOG_SILENT // GLUE_LOG_SILENT can be used to disable _all_ log messages.
                    // All logging can be disabled by calling
                    /// SetLogLevel(GLUE_LOG_SILENT).
};

//--------------------------------------------
// console colors for our logger - shame this doesn't work with the xcode console
#if GLUE_WINDOWS

    #define GLUE_CONSOLE_COLOR_RESTORE (0 | (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE))
    #define GLUE_CONSOLE_COLOR_BLACK (0)
    #define GLUE_CONSOLE_COLOR_RED (FOREGROUND_RED)
    #define GLUE_CONSOLE_COLOR_GREEN (FOREGROUND_GREEN)
    #define GLUE_CONSOLE_COLOR_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)
    #define GLUE_CONSOLE_COLOR_BLUE (FOREGROUND_BLUE)
    #define GLUE_CONSOLE_COLOR_PURPLE (FOREGROUND_RED | FOREGROUND_BLUE)
    #define GLUE_CONSOLE_COLOR_CYAN (FOREGROUND_GREEN | FOREGROUND_BLUE)
    #define GLUE_CONSOLE_COLOR_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#else

    #define GLUE_CONSOLE_COLOR_RESTORE (0)
    #define GLUE_CONSOLE_COLOR_BLACK (30)
    #define GLUE_CONSOLE_COLOR_RED (31)
    #define GLUE_CONSOLE_COLOR_GREEN (32)
    #define GLUE_CONSOLE_COLOR_YELLOW (33)
    #define GLUE_CONSOLE_COLOR_BLUE (34)
    #define GLUE_CONSOLE_COLOR_PURPLE (35)
    #define GLUE_CONSOLE_COLOR_CYAN (36)
    #define GLUE_CONSOLE_COLOR_WHITE (37)

#endif

/// \brief Sets the logging level to selectively show log messages.
///
/// This is useful if you want see lots of messages when debugging,
/// but then set a higher level so only warnings and errors appear for users.
///
/// LogLevel values in order from lowest to highest level are:
/// - `GLUE_LOG_DEBUG` (lowest level)
/// - `GLUE_LOG_INFO`
/// - `GLUE_LOG_WARN`
/// - `GLUE_LOG_ERROR`
/// - `GLUE_LOG_FATAL`
/// - `GLUE_LOG_SILENT` (highest level)
///
/// Thus, setting a log level of `GLUE_LOG_ERROR`, means only logging messages
/// marked GLUE_LOG_ERROR and GLUE_LOG_FATAL will be printed. Conversely,
/// setting GLUE_LOG_DEBUG means all log level messages, including
/// GLUE_LOG_DEBUG, will be printed.  Finally, setting a log level of
/// GLUE_LOG_SILENT will prevent any messages from being printed.
///
/// The default LogLevel is `GLUE_LOG_INFO`.
///
/// \param level the LogLevel (and below) you want to show
void SetLogLevel(LogLevel level);

/// \brief Set the logging level for a specific module.
///
/// When a module name is supplied to SetLogLevel, the provided LogLevel
/// is selectively applied only to Log messages marked with the specified
/// module.
///
/// This is particularly useful when the user desires to, for example, log at
/// an GLUE_LOG_DEBUG level for one module and then log at GLUE_LOG_ERROR for
/// another module.
///
/// Example of logging to a specific module:
///
/// ~~~~{.cpp}
/// // Set the default log level for all logging.
/// SetLogLevel(GLUE_LOG_ERROR);
///
/// // Selectively enable verbose logging for the MyClass module.
/// SetLogLevel("MyClass", GLUE_LOG_DEBUG);
///
/// // If we then log the following ...
///
/// // Log a vermose message to a module called "MyClass".
/// LogDebug("MyClass") << "A verbose message from MyClass.";
///
/// // Log a verbose message to a module called "MyOtherClass".
/// LogDebug("MyOtherClass") << "A verbose message from MyOtherClass.";
///
/// // In this case, we will see the verbose message from "MyClass", but not
/// // the message from "MyOtherClass".
/// ~~~~
void SetLogLevel(std::string module, LogLevel level);

/// \brief Get the currently set global logging level.
/// \returns The currently set global logging level.
LogLevel GetLogLevel();

/// \brief Get the logging level for a specific module.
/// \param module specific module name.
/// \returns The currently set specific module logging level.
LogLevel GetLogLevel(std::string module);

/// \brief Get log level name as a string.
/// \param level The LogLevel you want as a string.
/// \param pad True if you want all log level names to be the same length.
/// \returns The log level name as a string.
std::string GetLogLevelName(LogLevel level, bool pad = false);

/// \}

//--------------------------------------------------
/// \name Global logger channel
/// \{

class BaseLoggerChannel;

/// \brief Set the logging to output to a file instead of the console.
/// \param path The path to the log file to use.
/// \param append True if you want to append to the existing file.
void LogToFile(const std::string& path, bool append = false);

/// \brief Set the logging to ouptut to the console.
///
/// This is the default state and can be called to reset console logging
/// after LogToFile or SetLoggerChannel has been called.
void LogToConsole();

#if GLUE_WINDOWS
/// Set the logging to ouptut to windows debug view or visual studio console
///
/// This is the default state and can be called to reset console logging
/// after LogToFile or SetLoggerChannel has been called.
void LogToDebugView();
#endif

/// \brief Set the logger to use a custom logger channel.
///
/// Custom logger channels must extend BaseLoggerChannel. Custom log channels
/// can be useful for combining logging methods, logging to a server, logging
/// to email or even Twitter.
///
/// \param loggerChannel A shared pointer to the logger channel.
void SetLoggerChannel(std::shared_ptr<BaseLoggerChannel> loggerChannel);

/// \brief Get the current logger channel.
std::shared_ptr<BaseLoggerChannel> GetLoggerChannel();

/// \}

/// \class Log
/// \brief A C++ stream-style logging interface.
///
/// Log accepts variables via the std::ostream operator << and builds a string
/// and logs it when the stream is finished (via the destructor). A newline is
/// printed automatically and all the stream controls (std::endl, std::flush,
/// std::hex, etc) work normally. The default log level is `GLUE_LOG_INFO`.
///
/// Basic usage:
///
/// ~~~~{.cpp}
///
/// Log() << "My integer is " << 100 << " and my float is " << 20.234f;
///
/// ~~~~
///
/// It also accepts the legacy Log interface:
/// Log(LogLevel level, string message):
///
/// ~~~~{.cpp}
///
/// Log(GLUE_LOG_ERROR, "Another string.");
///
/// ~~~~
///
/// \author Dan Wilcox <danomatika@gmail.com> danomatika.com

// Class idea from
// http://www.gamedev.net/community/forums/topic.asp?topic_id=525405&whichpage=1&#3406418 How to
// catch std::endl (which is actually a func pointer)
// http://yvan.seth.id.au/Entries/Technology/Code/std__endl.html

class Log
{
public:
    /// \name Logging
    /// \{

    /// \brief Start logging on notice level.
    ///
    /// Log provides a streaming log interface by accepting variables via
    /// the `std::ostream` operator `<<` similar to `std::cout` and
    /// `std::cerr`.
    ///
    /// It builds a string and logs it when the stream is finished. A
    /// newline is printed automatically and all the stream controls
    /// (`std::endl`, `std::flush`, `std::hex`, etc)
    /// work normally.
    ///
    /// ~~~~{.cpp}
    ///
    /// // Converts primitive types (int, float, etc) to strings automatically.
    /// Log() << "a string " << 100 << 20.234f;
    ///
    /// ~~~~
    ///
    /// The log level is `GLUE_LOG_INFO` by default.
    Log();

    /// \brief Start logging on a specific LogLevel.
    ///
    /// Example:
    /// ~~~~{.cpp}
    ///
    /// // Set the log level.
    /// Log(GLUE_LOG_WARN) << "a string " << 100 << 20.234f;
    ///
    /// ~~~~
    ///
    /// You can use the derived convenience classes as an alternative for specific log levels:
    ///
    /// 	LogDebug()
    /// 	LogInfo()
    /// 	LogWarn()
    /// 	LogError()
    /// 	LogFatal()
    ///
    /// ~~~~{.cpp}
    ///
    /// // Set the log level.
    /// Log(GLUE_LOG_WARN) << "a string " << 100 << 20.234f;
    ///
    /// // This is the same as above.
    /// LogWarn() << "a string " << 100 << 20.234f;
    ///
    /// ~~~~
    ///
    /// \param level The LogLevel for this log message.
    Log(LogLevel level);

    /// \brief Log a string at a specific log level.
    ///
    /// Supply the logging message as a parameter to the function
    /// instead of as a stream.
    ///
    /// The string message can be concatenated using the
    /// ofToString(const T& value) conversion function:
    ///
    /// ~~~~{.cpp}
    ///
    /// // Build a single string message.
    /// Log(GLUE_LOG_INFO, "the number is "
    /// + ofToString(10) + " and I have a float too " + ofToString(123.45f));
    ///
    /// ~~~~
    ///
    /// \param level The LogLevel for this log message.
    /// \param message The log message.
    Log(LogLevel level, const std::string& message);

    /// \brief Logs a message at a specific log level using the printf interface.
    ///
    /// The message is built using the formatting from the C printf function
    /// and can be used as a direct replacement. Essentially, the second
    /// argument is a string with special formatting specifiers starting
    /// with '%' that specify where the following variables go in the
    /// message. You can have as many variables as you want following the
    /// logLevel and format string, but there must be a % specifier for each
    /// subsequent variable.
    ///
    /// For quick reference, here are a few of the most useful formatting
    /// specifiers:
    ///
    /// * `%d`: integer number, `123`
    /// * `%f`: floating point number, `123.45`
    /// * `%s`: a C string ([null terminated](http://en.wikipedia.org/wiki/Null-terminated_string));
    /// this is not a C++ string, use
    /// [string::c_str()](http://www.cplusplus.com/reference/string/string/c_str/) to get a C string
    /// from a C++ string
    /// * `%c`: a single character
    /// * `%x`: unsigned integer as a [hexidecimal](http://en.wikipedia.org/wiki/Hexadecimal)
    /// number; `x` uses lower-case letters and `X` uses upper-case
    /// * `%%`: prints a `%` character
    ///
    /// The specifier should match the variable type as it is used to tell
    /// the function how to convert that primitive type (int, float,
    /// character, etc) into a string.
    ///
    /// For instance, let's say we want to print two messages, a salutation
    /// and the value of an int, a float, and a string variable:
    ///
    /// ~~~~{.cpp}
    ///
    /// // Print a simple message with no variables.
    /// Log(GLUE_LOG_WARN, "Welcome to the jungle.");
    ///
    /// // Our variables.
    /// float fun = 11.11;
    /// int games = 100;
    /// string theNames = "Dan, Kyle, & Golan";
    ///
    /// // Print a message with variables, sets the message format in the
    /// // format string.
    /// Log(GLUE_LOG_INFO, "we've got %d & %f, we got everything you want honey, we know %s", fun,
    /// games, theNames.c_str());
    ///
    /// ~~~~
    ///
    /// Note: `theNames.c_str()` returns a C string from theNames which is
    /// a C++ string object.
    ///
    /// There are other formatting options such as setting the decimal
    /// precision of float objects and the forward padding of numbers
    /// (i.e. 0001 instead of 1). See the [Wikipedia printf format string
    /// article](http://en.wikipedia.org/wiki/Printf_format_string) for more
    /// detailed information.
    ///
    /// \param level The LogLevel for this log message.
    /// \param format The printf-style format string.
    template <typename... Args>
    Log(LogLevel level, const char* format, Args&&... args)
        : Log(level, ofVAArgsToString(format, args...))
    {}
    /// \}

    //--------------------------------------------------
    /// \name Logging configuration
    /// \{

    /// \brief Let the logger automaticly add spaces between messages.
    ///
    /// Default is `false`.
    ///
    /// \param autoSpace Set to true to add spaces between messages
    static void setAutoSpace(bool autoSpace);

    /// \brief Set the logging channel destinations for messages.
    ///
    /// This can be used to output to files instead of stdout.
    ///
    /// \sa FileLoggerChannel ConsoleLoggerChannel
    /// \param channel The channel to log to.
    static void setChannel(std::shared_ptr<BaseLoggerChannel> channel);

    /// \brief Get the current logging channel.
    static std::shared_ptr<BaseLoggerChannel> getChannel();

    /// \}

    /// \cond INTERNAL

    /// \brief Destroy the Log.
    ///
    /// This destructor does the actual printing via std::ostream.
    virtual ~Log();

    /// \brief Define flexible stream operator.
    ///
    /// This allows the class to use the << std::ostream to read data of
    /// almost any type.
    ///
    /// \tparam T the data type to be streamed.
    /// \param value the data to be streamed.
    /// \returns A reference to itself.
    template <class T>
    Log& operator<<(const T& value)
    {
        message << value << getPadding();
        return *this;
    }

    /// \brief Define flexible stream operator.
    ///
    /// This allows the class to use the << std::ostream to catch function
    /// pointers such as std::endl and std::hex.
    ///
    /// \param func A function pointer that takes a std::ostream as an argument.
    /// \returns A reference to itself.
    Log& operator<<(std::ostream& (*func)(std::ostream&) )
    {
        func(message);
        return *this;
    }

    /// \endcond

protected:
    /// \cond INTERNAL

    LogLevel level;     ///< Log level.
    bool bPrinted;      ///< Has the message been printed in the constructor?
    std::string module; ///< The destination module for this message.

    /// \brief Print a log line.
    /// \param level The log level.
    /// \param module The target module.
    /// \param message The log message.
    void _log(LogLevel level, const std::string& module, const std::string& message);

    /// \brief Determine if the given module is active at the given log level.
    /// \param level The log level.
    /// \param module The target module.
    /// \returns true if the given module is active at the given log level.
    bool checkLog(LogLevel level, const std::string& module);

    static std::shared_ptr<BaseLoggerChannel>& channel(); ///< The target channel.

    /// \endcond

private:
    std::stringstream message; ///< Temporary buffer.

    static bool bAutoSpace; ///< Should space be added between messages?

    Log(Log const&) {}                          // not defined, not copyable
    Log& operator=(Log& from) { return *this; } // not defined, not assignable

    static std::string& getPadding(); ///< The padding between std::ostream calls.
};

/// \brief Derived log class for easy verbose logging.
///
/// Example: `LogDebug("Log message")`.
class LogDebug : public Log
{
public:
    /// \brief Create a verbose log message.
    /// \param module The target module.
    LogDebug(const std::string& module = "");

    /// \brief Create a verbose log message.
    /// \param module The target module.
    /// \param message The log message.
    LogDebug(const std::string& module, const std::string& message);

    /// \brief Create a verbose log message.
    /// \param module The target module.
    /// \param format The printf-style format string.
    template <typename... Args>
    LogDebug(const std::string& module, const char* format, Args&&... args)
        : LogDebug(module, ofVAArgsToString(format, args...))
    {}
};

/// \brief Derived log class for easy notice logging.
///
/// Example: `LogInfo("Log message")`.
class LogInfo : public Log
{
public:
    /// \brief Create a notice log message.
    /// \param module The target module.
    LogInfo(const std::string& module = "");

    /// \brief Create a notice log message.
    /// \param module The target module.
    /// \param message The log message.
    LogInfo(const std::string& module, const std::string& message);

    /// \brief Create a notice log message.
    /// \param module The target module.
    /// \param format The printf-style format string.
    template <typename... Args>
    LogInfo(const std::string& module, const char* format, Args&&... args)
        : LogInfo(module, ofVAArgsToString(format, args...))
    {}
};

/// \brief Derived log class for easy warning logging.
///
/// Example: `LogWarn("Log message")`.
class LogWarn : public Log
{
public:
    /// \brief Create a verbose log message.
    /// \param module The target module.
    LogWarn(const std::string& module = "");
    /// \brief Create a verbose log message.
    /// \param module The target module.
    /// \param message The log message.
    LogWarn(const std::string& module, const std::string& message);

    /// \brief Create a verbose log message.
    /// \param module The target module.
    /// \param format The printf-style format string.
    template <typename... Args>
    LogWarn(const std::string& module, const char* format, Args&&... args)
        : LogWarn(module, ofVAArgsToString(format, args...))
    {}
};

/// \brief Derived log class for easy error logging.
///
/// Example: `LogError("Log message")`.
class LogError : public Log
{
public:
    /// \brief Create a error log message.
    /// \param module The target module.
    LogError(const std::string& module = "");

    /// \brief Create a error log message.
    /// \param module The target module.
    /// \param message The log message.
    LogError(const std::string& module, const std::string& message);

    /// \brief Create a error log message.
    /// \param module The target module.
    /// \param format The printf-style format string.
    template <typename... Args>
    LogError(const std::string& module, const char* format, Args&&... args)
        : LogError(module, ofVAArgsToString(format, args...))
    {}
};

/// \brief Derived log class for easy fatal error logging.
///
/// Example: `LogFatal("Log message")`.
class LogFatal : public Log
{
public:
    /// \brief Create a fatal error log message.
    /// \param module The target module.
    LogFatal(const std::string& module = "");

    /// \brief Create a fatal error log message.
    /// \param module The target module.
    /// \param message The log message.
    LogFatal(const std::string& module, const std::string& message);

    /// \brief Create a fatal error log message.
    /// \param module The target module.
    /// \param format The printf-style format string.
    template <typename... Args>
    LogFatal(const std::string& module, const char* format, Args&&... args)
        : LogFatal(module, ofVAArgsToString(format, args...))
    {}
};

/// \cond INTERNAL

//--------------------------------------------------------------
// Logger Channels

/// \brief The base class representing a logger channel.
///
/// Users can derive their own logging channels from BaseLoggerChannel or use
/// default channels.
class BaseLoggerChannel
{
public:
    /// \brief Destroy the channel.
    virtual ~BaseLoggerChannel(){};

    /// \brief Log a message.
    /// \param level The log level.
    /// \param module The target module.
    /// \param message The log message.
    virtual void log(LogLevel level, const std::string& module, const std::string& message) = 0;

    /// \brief Log a message.
    /// \param level The log level.
    /// \param module The target module.
    /// \param format The printf-style format string.
    template <typename... Args>
    void log(LogLevel level, const std::string& module, const char* format, Args&&... args)
    {
        log(level, module, ofVAArgsToString(format, args...));
    }
};

/// \brief A logger channel that logs its messages to the console.
class ConsoleLoggerChannel : public BaseLoggerChannel
{
public:
    /// \brief Destroy the console logger channel.
    virtual ~ConsoleLoggerChannel(){};
    void log(LogLevel level, const std::string& module, const std::string& message);
};

#if GLUE_ANDROID
class AndroidLogChannel : public BaseLoggerChannel
{
public:
    virtual ~AndroidLogChannel(){};

    void log(LogLevel level, const std::string& module, const std::string& msg);
};
#endif

#if GLUE_WINDOWS
/// A logger channel that logs its messages to windows debug view and visual studio output.
class DebugViewLoggerChannel : public BaseLoggerChannel
{
public:
    /// \brief Destroy the console logger channel.
    virtual ~DebugViewLoggerChannel(){};
    void log(LogLevel level, const std::string& module, const std::string& message);
};
#endif

/// \brief A logger channel that logs its messages to a log file.
class FileLoggerChannel : public BaseLoggerChannel
{
public:
    /// \brief Create an FileLoggerChannel.
    FileLoggerChannel();

    /// \brief Create an FileLoggerChannel with parameters.
    /// \param path The file path for the log file.
    /// \param append True if the log data should be added to an existing file.
    FileLoggerChannel(const std::string& path, bool append);

    /// \brief Destroy the file logger channel.
    virtual ~FileLoggerChannel();

    /// \brief Set the log file.
    /// \param path The file path for the log file.
    /// \param append True if the log data should be added to an existing file.
    void setFile(const std::string& path, bool append = false);

    void log(LogLevel level, const std::string& module, const std::string& message);

    /// \brief CLose the log file.
    void close();

private:
    File m_file; ///< The location of the log file.
};

/// \endcond

} // namespace glue
