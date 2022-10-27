//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "time/PerformanceCounter.h"

#include "maths/MathsFunctions.h"

namespace glue
{

static void appendToFile(const File& f, const String& s)
{
    if (f.getFullPathName().isNotEmpty())
    {
        FileOutputStream out(f);

        if (!out.failedToOpen()) out << s << newLine;
    }
}

PerformanceCounter::PerformanceCounter(const String& name, int runsPerPrintout,
                                       const File& loggingFile)
    : runsPerPrint(runsPerPrintout), startTime(0), outputFile(loggingFile)
{
    stats.name = name;
    appendToFile(outputFile, "**** Counter for \"" + name +
                                 "\" started at: " + Time::getCurrentTime().toString(true, true));
}

PerformanceCounter::~PerformanceCounter()
{
    if (stats.numRuns > 0) printStatistics();
}

PerformanceCounter::Statistics::Statistics() noexcept
    : averageSeconds(), maximumSeconds(), minimumSeconds(), totalSeconds(), numRuns()
{}

void PerformanceCounter::Statistics::clear() noexcept
{
    averageSeconds = maximumSeconds = minimumSeconds = totalSeconds = 0;
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
        maximumSeconds = jmax(maximumSeconds, elapsed);
        minimumSeconds = jmin(minimumSeconds, elapsed);
    }

    ++numRuns;
    totalSeconds += elapsed;
}

static String timeToString(double secs)
{
    return String((int64) (secs * (secs < 0.01 ? 1000000.0 : 1000.0) + 0.5)) +
           (secs < 0.01 ? " microsecs" : " millisecs");
}

String PerformanceCounter::Statistics::toString() const
{
    std::stringstream s;

    s << "Performance count for \"" << name << "\" over " << numRuns << " run(s)"
      << "\n"
      << "Average = " << timeToString(averageSeconds)
      << ", minimum = " << timeToString(minimumSeconds)
      << ", maximum = " << timeToString(maximumSeconds)
      << ", total = " << timeToString(totalSeconds);

    return String(s.str());
}

void PerformanceCounter::start() noexcept { startTime = Time::getHighResolutionTicks(); }

bool PerformanceCounter::stop()
{
    stats.addResult(Time::highResolutionTicksToSeconds(Time::getHighResolutionTicks() - startTime));

    if (stats.numRuns < runsPerPrint) return false;

    printStatistics();
    return true;
}

void PerformanceCounter::printStatistics()
{
    const String desc(getStatisticsAndReset().toString());

    Logger::writeToLog(desc);
    appendToFile(outputFile, desc);
}

PerformanceCounter::Statistics PerformanceCounter::getStatisticsAndReset()
{
    Statistics s(stats);
    stats.clear();

    if (s.numRuns > 0) s.averageSeconds = s.totalSeconds / (float) s.numRuns;

    return s;
}

} // namespace glue
