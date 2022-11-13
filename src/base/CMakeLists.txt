#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

set(source
  barrier.cpp
  countdown_latch.cpp
  exception.cpp
  file_mapping.cpp
  file.cpp
  fps_counter.cpp
  log.cpp
  object_pool.cpp
  performance_counter.cpp
  spin_lock.cpp
  thread_pool.cpp
  thread.cpp
  time.cpp
  version.cpp
  waitable_event.cpp
)

set(target base)

add_library(${target} SHARED ${source})
add_library(glue::base ALIAS ${target})

target_include_directories(${target}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(${target} PROPERTIES
    VERSION         "${PROJECT_VERSION}"
    SOVERSION       "${PROJECT_VERSION_MAJOR}"
    DEFINE_SYMBOL   GLUE_EXPORTS
)

if(GLUE_ENABLE_INSTALL)
  install(TARGETS ${target}
    EXPORT  ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${ANDROID_ABI}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${ANDROID_ABI}
  )
endif()
