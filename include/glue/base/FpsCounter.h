//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/StandardHeader.h"

GLUE_START_NAMESPACE

class GLUE_API FpsCounter final
{
public:
    FpsCounter();
    FpsCounter(double targetFps);
    void newFrame();

    // no need to call it usually, useful if
    // the update rate of the source is not very
    // consistent. calling update from the main
    // loop for example will make the measurement
    // update more often
    void update();
    double getFps() const;
    uint64 getNumFrames() const;
    void setFilterAlpha(float alpha);

private:
    void update(double now);

private:
    uint64 m_nFrameCount;
    int64 m_then;
    int64 m_lastFrameTime;
    int64 m_filteredTime;
    double m_fps;
    double m_filterAlpha;
    std::queue<double> m_timestamps;
};

GLUE_END_NAMESPACE
