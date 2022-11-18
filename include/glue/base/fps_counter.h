//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.h"

#include <queue>

namespace glue {

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
    uint64_t getNumFrames() const;
    void setFilterAlpha(float alpha);

private:
    void update(double now);

private:
    uint64_t m_nFrameCount;
    int64_t m_then;
    int64_t m_lastFrameTime;
    int64_t m_filteredTime;
    double m_fps;
    double m_filterAlpha;
    std::queue<double> m_timestamps;
};

} // namespace glue
