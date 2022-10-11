//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/log/record.h"
#include "glue/log/sink.h"

#include <fstream>
#include <mutex>

namespace glue
{
namespace log
{

    template <class Formatter>
    class FileSink : public LogSink
    {
    public:
        FileSink(const char* filename)
        {
            m_outputStream.open(filename, std::fstream::out | std::fstream::trunc);
        }

        void write(const Record& record) override
        {
            std::string str = Formatter::format(record);

            std::lock_guard<std::mutex> lck(m_mutex);
            m_outputStream << str << std::flush;
        }

    private:
        std::mutex m_mutex;
        std::ofstream m_outputStream;
    };

} // namespace log
} // namespace glue
