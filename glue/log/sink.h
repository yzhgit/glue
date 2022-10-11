//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

// #include "glue/log/record.h"

namespace glue
{
namespace log
{

    class Record;

    /** Base Appender class to be inherited by other Appender classes */
    class LogSink
    {
    public:
        /** Defaults Destructor */
        virtual ~LogSink() = default;

        /** Print message
         *
         * @param[in] msg Message to print
         */
        virtual void write(const Record& record) = 0;
    };

} // namespace log
} // namespace glue
