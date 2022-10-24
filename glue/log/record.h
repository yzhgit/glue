//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/log/severity.h"
#include "glue/log/util.h"

#include <cstdarg>
#include <string>
#include <time.h>
#include <vector>

namespace glue
{
namespace log
{

    class Record
    {
    public:
        Record(Severity severity, size_t line, const char* file)
            : m_severity(severity), m_tid(util::gettid()), m_line(line), m_file(file)
        {
            m_content.reserve(1024);
            timespec_get(&m_time, TIME_UTC);
        }

        template <typename T>
        Record& operator<<(const T& value)
        {
            static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value,
                          "is not a number type.");
            m_content.append(std::to_string(value));
            return *this;
        }

        Record& operator<<(const char* str)
        {
            if (str) { m_content.append(str); }
            return *this;
        }
        Record& operator<<(char* str)
        {
            if (str) { m_content.append(str); }
            return *this;
        }
        Record& operator<<(const std::string& str)
        {
            m_content.append(str);
            return *this;
        }

        //////////////////////////////////////////////////////////////////////////
        // Getters

        const struct timespec& getTime() const { return m_time; }

        Severity getSeverity() const { return m_severity; }

        size_t getTid() const { return m_tid; }

        size_t getLine() const { return m_line; }

        const char* getMessage() const { return m_content.c_str(); }

        const char* getFile() const { return m_file; }

        virtual ~Record() // virtual destructor to satisfy -Wnon-virtual-dtor
                          // warning
        {}

    private:
        struct timespec m_time;
        const Severity m_severity;
        const size_t m_tid;
        const size_t m_line;
        const char* const m_file;
        std::string m_content;
    };

} // namespace log
} // namespace glue
