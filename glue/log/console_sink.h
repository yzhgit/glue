//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/log/record.h"
#include "glue/log/sink.h"

#include <iostream>
#include <mutex>

namespace glue
{
namespace log
{

    template <class Formatter>
    class ConsoleSink : public LogSink
    {
    public:
        ConsoleSink() : m_outputStream(std::cout) {}

        void write(const Record& record) override
        {
            std::string str = Formatter::format(record);

            std::lock_guard<std::mutex> lck(m_mutex);
            m_outputStream << str << std::flush;
        }

    private:
        std::mutex m_mutex;
        std::ostream& m_outputStream;
    };

} // namespace log
} // namespace glue
