#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

option(GLUE_USE_NCNN "Build with ncnn" OFF)
option(GLUE_USE_SNPE "Build with snpe" OFF)
option(GLUE_USE_TENSORRT "Build with tensorrt" ON)

set(source
  predictor.cpp
  tensor.cpp
)

set(target nn)

add_library(${target} SHARED ${source})
add_library(glue::nn ALIAS ${target})

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
