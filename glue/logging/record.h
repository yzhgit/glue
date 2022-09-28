//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/logging/helpers.h"
#include "glue/logging/severity.h"

#include <cstdarg>
#include <string>

namespace glue {
namespace log {

class Record {
  public:
    Record(Severity severity, size_t line, const char *file)
        : m_severity(severity), m_tid(util::gettid()), m_line(line),
          m_file(file) {
        util::ftime(&m_time);
    }

    Record &ref() { return *this; }

    //////////////////////////////////////////////////////////////////////////
    // Stream output operators

    Record &operator<<(char data) {
        char str[] = {data, 0};
        return *this << str;
    }

    Record &operator<<(std::ostream &(*data)(std::ostream &)) {
        m_message << data;
        return *this;
    }

    template <typename T> Record &operator<<(const T &data) {
        using namespace plog::detail;

        m_message << data;
        return *this;
    }

    Record &printf(const char *format, ...) {
        using namespace util;

        char *str = NULL;
        va_list ap;

        va_start(ap, format);
        int len = vasprintf(&str, format, ap);
        static_cast<void>(len);
        va_end(ap);

        *this << str;
        free(str);

        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // Getters

    const util::Time &getTime() const { return m_time; }

    Severity getSeverity() const { return m_severity; }

    size_t getTid() const { return m_tid; }

    size_t getLine() const { return m_line; }

    const util::nchar *getMessage() const {
        m_messageStr = m_message.str();
        return m_messageStr.c_str();
    }

    const char *getFile() const { return m_file; }

    virtual ~Record() // virtual destructor to satisfy -Wnon-virtual-dtor
                      // warning
    {}

  private:
    util::Time m_time;
    const Severity m_severity;
    const size_t m_tid;
    const size_t m_line;
    const char *const m_file;
    util::nostringstream m_message;
    mutable util::nstring m_messageStr;
};

} // namespace log
} // namespace glue
