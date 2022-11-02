//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/StandardHeader.h"

namespace glue
{

class GLUE_API FpsCounter
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
    uint64 nFrameCount;
    int64 then;
    int64 lastFrameTime;
    int64 filteredTime;
    double fps;
    double filterAlpha;
    std::queue<double> timestamps;
};

} // namespace glue
