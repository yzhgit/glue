#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

set(source
  aes.cpp
  base64.cpp
  hex.cpp
  md5.cpp
  mem_ops.cpp
  poly1305.cpp
  salsa20.cpp
  sha256.cpp
  sha512.cpp
  ed25519/ed25519_fe.cpp
  ed25519/ed25519.cpp
  ed25519/ge.cpp
  ed25519/sc_muladd.cpp
  ed25519/sc_reduce.cpp
)

set(target crypto)

add_library(${target} SHARED ${source})
add_library(glue::crypto ALIAS ${target})

target_include_directories(${target}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
