//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "utils/FpsCounter.h"

#include "time/Time.h"

namespace glue
{

FpsCounter::FpsCounter()
    : nFrameCount(0)
    , then(Time::currentTimeMillis())
    , fps(0)
    , lastFrameTime(0)
    , filteredTime(0)
    , filterAlpha(0.9)
{}

FpsCounter::FpsCounter(double targetFPS)
    : nFrameCount(0)
    , then(Time::currentTimeMillis())
    , fps(targetFPS)
    , lastFrameTime(0)
    , filteredTime(0)
    , filterAlpha(0.9)
{}

void FpsCounter::newFrame()
{
    auto now = Time::currentTimeMillis();
    double nowInSeconds = now / 1000;

    update(nowInSeconds);
    timestamps.push(nowInSeconds);

    lastFrameTime = now - then;
    filteredTime = filteredTime * filterAlpha + lastFrameTime * (1 - filterAlpha);
    then = now;
    nFrameCount++;
}

void FpsCounter::update() { update(Time::currentTimeMillis() / 1000); }

void FpsCounter::update(double now)
{
    while (!timestamps.empty() && timestamps.front() + 2 < now) { timestamps.pop(); }

    auto diff = 0.0;
    if (!timestamps.empty() && timestamps.front() + 0.5 < now) { diff = now - timestamps.front(); }
    if (diff > 0.0) { fps = timestamps.size() / diff; }
    else
    {
        fps = timestamps.size();
    }
}

double FpsCounter::getFps() const { return fps; }

uint64_t FpsCounter::getNumFrames() const { return nFrameCount; }

void FpsCounter::setFilterAlpha(float alpha) { filterAlpha = alpha; }

} // namespace glue
