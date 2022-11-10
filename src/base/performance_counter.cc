//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/performance_counter.hpp"

#include "glue/base/log.hpp"

namespace glue {

PerformanceCounter::PerformanceCounter(const char* name, int runsPerPrintout,
                                       const char* loggingFile)
    : m_runsPerPrint(runsPerPrintout), m_startTime(0)
{
    m_stats.name = name;
    if (loggingFile)
        m_outputFile = fopen(loggingFile, "w");
    else
        m_outputFile = stdout;

    time_t now;
    ::time(&now);
    fprintf(m_outputFile, "**** Counter for \"%s\" started at: %s\n", name, ctime(&now));
}

PerformanceCounter::~PerformanceCounter()
{
    if (m_stats.numRuns > 0) printStatistics();
    if (m_outputFile && m_outputFile != stdout) { fclose(m_outputFile); }
}

PerformanceCounter::Statistics::Statistics() noexcept
    : averageSeconds(), maximumSeconds(), minimumSeconds(), totalSeconds(), numRuns()
{}

void PerformanceCounter::Statistics::clear() noexcept
{
    averageSeconds = 0;
    maximumSeconds = 0;
    minimumSeconds = 0;
    totalSeconds = 0;
    numRuns = 0;
}

void PerformanceCounter::Statistics::addResult(double elapsed) noexcept
{
    if (numRuns == 0)
    {
        maximumSeconds = elapsed;
        minimumSeconds = elapsed;
    }
    else
    {
        maximumSeconds = std::max(maximumSeconds, elapsed);
        minimumSeconds = std::min(minimumSeconds, elapsed);
    }

    ++numRuns;
    totalSeconds += elapsed;
}

static std::string timeToString(double secs)
{
    return std::to_string((int64) (secs * (secs < 0.01 ? 1000000.0 : 1000.0) + 0.5)) +
           (secs < 0.01 ? " microsecs" : " millisecs");
}

std::string PerformanceCounter::Statistics::toString() const
{
    std::stringstream s;

    s << "Performance count for \"" << name << "\" over " << numRuns << " run(s)"
      << "\n"
      << "Average = " << timeToString(averageSeconds)
      << ", minimum = " << timeToString(minimumSeconds)
      << ", maximum = " << timeToString(maximumSeconds)
      << ", total = " << timeToString(totalSeconds);

    return s.str();
}

void PerformanceCounter::start() noexcept
{
    m_startTime = Time::getHighResolutionTicks();
}

bool PerformanceCounter::stop()
{
    m_stats.addResult(
        Time::highResolutionTicksToSeconds(Time::getHighResolutionTicks() - m_startTime));

    if (m_stats.numRuns < m_runsPerPrint) return false;

    printStatistics();
    return true;
}

void PerformanceCounter::printStatistics()
{
    std::string desc = getStatisticsAndReset().toString();

    LogInfo() << desc;
    fprintf(m_outputFile, "%s\n", desc.c_str());
}

PerformanceCounter::Statistics PerformanceCounter::getStatisticsAndReset()
{
    Statistics s(m_stats);
    m_stats.clear();

    if (s.numRuns > 0) s.averageSeconds = s.totalSeconds / (float) s.numRuns;

    return s;
}

}
