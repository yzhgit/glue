//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/log/record.h"
#include "glue/log/util.h"

#include <iomanip>
#include <sstream>

namespace glue
{
namespace log
{

    class RawFormatter
    {
    public:
        static std::string format(const Record& record)
        {
            std::stringstream ss;
            ss << record.getMessage() << "\n";

            return ss.str();
        }
    };

    template <bool useUtcTime>
    class DefaultFormatterImpl
    {
    public:
        static std::string format(const Record& record)
        {
            tm t;
            useUtcTime ? util::gmtime_s(&t, &record.getTime().tv_sec)
                       : util::localtime_s(&t, &record.getTime().tv_sec);

            std::stringstream ss;
            ss << "[" << t.tm_year + 1900 << "-" << std::setfill('0') << std::setw(2)
               << t.tm_mon + 1 << "-" << std::setfill('0') << std::setw(2) << t.tm_mday << " ";
            ss << std::setfill('0') << std::setw(2) << t.tm_hour << ":" << std::setfill('0')
               << std::setw(2) << t.tm_min << ":" << std::setfill('0') << std::setw(2) << t.tm_sec
               << "." << std::setfill('0') << std::setw(3)
               << static_cast<int>(record.getTime().millitm) << " ";
            ss << std::setfill(' ') << std::setw(5) << std::left
               << severityToString(record.getSeverity()) << " " << record.getTid() << " "
               << record.getFile() << ":" << record.getLine() << "] " << record.getMessage()
               << "\n";

            return ss.str();
        }
    };

    class DefaultFormatter : public DefaultFormatterImpl<false>
    {
    };
    class DefaultFormatterUtc : public DefaultFormatterImpl<true>
    {
    };

} // namespace log
} // namespace glue
