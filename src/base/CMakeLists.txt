#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

set(source
  Barrier.cpp
  CountDownLatch.cpp
  CpuInfo.cpp
  Event.cpp
  Exception.cpp
  File.cpp
  FpsCounter.cpp
  Log.cpp
  MemoryMappedFile.cpp
  PerformanceCounter.cpp
  SpinLock.cpp
  Thread.cpp
  ThreadPool.cpp
  Time.cpp
  Version.cpp
)

set(target base)

add_library(${target} SHARED ${source})

target_precompile_headers(${target}
  PUBLIC
    <algorithm>
    <array>
    <atomic>
    <cassert>
    <cmath>
    <condition_variable>
    <cstddef>
    <ctime>
    <fstream>
    <functional>
    <iomanip>
    <iostream>
    <limits>
    <list>
    <map>
    <memory>
    <mutex>
    <numeric>
    <queue>
    <set>
    <sstream>
    <stdarg.h>
    <stdexcept>
    <string>
    <thread>
    <typeindex>
    <unordered_map>
    <unordered_set>
    <utility>
    <vector>
)

set_target_properties(${target} PROPERTIES
    VERSION         "${PROJECT_VERSION}"
    SOVERSION       "${PROJECT_VERSION_MAJOR}"
    DEFINE_SYMBOL   GLUE_EXPORTS
)

if(GL_ENABLE_INSTALL)
  install(TARGETS ${target}
    EXPORT  ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${ANDROID_ABI}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${ANDROID_ABI}
  )
endif()
