//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "glue/base/FpsCounter.h"

#include "glue/base/Time.h"

namespace glue
{

FpsCounter::FpsCounter()
    : m_nFrameCount(0)
    , m_then(Time::currentTimeMillis())
    , m_fps(0)
    , m_lastFrameTime(0)
    , m_filteredTime(0)
    , m_filterAlpha(0.9)
{}

FpsCounter::FpsCounter(double targetFPS)
    : m_nFrameCount(0)
    , m_then(Time::currentTimeMillis())
    , m_fps(targetFPS)
    , m_lastFrameTime(0)
    , m_filteredTime(0)
    , m_filterAlpha(0.9)
{}

void FpsCounter::newFrame()
{
    auto now = Time::currentTimeMillis();
    double nowInSeconds = now / 1000;

    update(nowInSeconds);
    m_timestamps.push(nowInSeconds);

    m_lastFrameTime = now - m_then;
    m_filteredTime = m_filteredTime * m_filterAlpha + m_lastFrameTime * (1 - m_filterAlpha);
    m_then = now;
    m_nFrameCount++;
}

void FpsCounter::update() { update(Time::currentTimeMillis() / 1000); }

void FpsCounter::update(double now)
{
    while (!m_timestamps.empty() && m_timestamps.front() + 2 < now) { m_timestamps.pop(); }

    auto diff = 0.0;
    if (!m_timestamps.empty() && m_timestamps.front() + 0.5 < now)
    {
        diff = now - m_timestamps.front();
    }
    if (diff > 0.0) { m_fps = m_timestamps.size() / diff; }
    else
    {
        m_fps = m_timestamps.size();
    }
}

double FpsCounter::getFps() const { return m_fps; }

uint64_t FpsCounter::getNumFrames() const { return m_nFrameCount; }

void FpsCounter::setFilterAlpha(float alpha) { m_filterAlpha = alpha; }

} // namespace glue
