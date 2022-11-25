//
// Copyright © 2017 yaozhang. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include <tuple>
#include <utility>

#include "glue/cv/thread_pool.h"

#ifdef LITE_USE_THREAD_POOL
/* support basic for loop
 * for (int i = 0; i < work_size; ++i)
 */
#define LITE_PARALLEL_BEGIN(index, tid, work_size)          \
    {                                                       \
        std::pair<std::function<void(int, int)>, int> task; \
        task.second = work_size;                            \
        task.first = [&](int index, int tid) {
#define LITE_PARALLEL_END()                     \
    }                                           \
    ;                                           \
    glue::ThreadPool::Enqueue(std::move(task)); \
    }

/* support common for loop
 * for (int i = start; i < end; i += step)
 */
#define LITE_PARALLEL_COMMON_BEGIN(index, tid, end, start, step)       \
    {                                                                  \
        std::tuple<std::function<void(int, int)>, int, int, int> task; \
        std::get<3>(task) = step;                                      \
        std::get<2>(task) = start;                                     \
        std::get<1>(task) = end;                                       \
        std::get<0>(task) = [&](int index, int tid) {
#define LITE_PARALLEL_COMMON_END()              \
    }                                           \
    ;                                           \
    glue::ThreadPool::Enqueue(std::move(task)); \
    }

#elif defined(ARM_WITH_OMP)
#include <omp.h>

#define LITE_PARALLEL_BEGIN(index, tid, work_size) \
    _Pragma("omp parallel for") for (int index = 0; index < (work_size); ++index) {
#define LITE_PARALLEL_END() }

#define LITE_PARALLEL_COMMON_BEGIN(index, tid, end, start, step) \
    _Pragma("omp parallel for") for (int index = (start); index < (end); index += (step)) {
#define LITE_PARALLEL_COMMON_END() }

#else
#define LITE_PARALLEL_BEGIN(index, tid, work_size)      \
    for (int index = 0; index < (work_size); ++index) { \
#define LITE_PARALLEL_END()                     \
    }

#define LITE_PARALLEL_COMMON_BEGIN(index, tid, end, start, step) \
    for (int index = (start); index < (end); index += (step)) {
#define LITE_PARALLEL_COMMON_END() }
#endif
