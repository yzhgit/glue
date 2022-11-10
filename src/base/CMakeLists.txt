#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

set(source
  barrier.cc
  countdown_latch.cc
  exception.cc
  file_mapping.cc
  file.cc
  fps_counter.cc
  log.cc
  object_pool.cc
  performance_counter.cc
  spin_lock.cc
  thread_pool.cc
  thread.cc
  time.cc
  version.cc
  waitable_event.cc
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
