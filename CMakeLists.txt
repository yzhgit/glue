#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.17.0)

if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")

##############################################################################
# Project
##############################################################################

set(GLUE_VERSION_MAJOR 0 CACHE STRING "major version of GLUE" FORCE)
set(GLUE_VERSION_MINOR 0 CACHE STRING "minor version of GLUE" FORCE)
set(GLUE_VERSION_PATCH 1 CACHE STRING "patch version of GLUE" FORCE)

set(GLUE_VERSION "${GLUE_VERSION_MAJOR}.${GLUE_VERSION_MINOR}.${GLUE_VERSION_PATCH}" CACHE STRING "version of GLUE" FORCE)

project(glue
  VERSION "${GLUE_VERSION}"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(BUILD_SHARED_LIBS)
  add_compile_options(-fvisibility=hidden)
endif(BUILD_SHARED_LIBS)

# Output directory is correct by default for most build setups. However, when
# building as a DLL, it is important to have the DLL in the same
# directory as the executable using it. Thus, we put all executables in a single
# /bin directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Determine whether this is a standalone project or included by other projects
set(GLUE_STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(GLUE_STANDALONE_PROJECT ON)
endif()

### Project options
option(GLUE_ENABLE_INSTALL "Generate and install GLUE target" ${GLUE_STANDALONE_PROJECT})
option(GLUE_ENABLE_TEST "Build and perform GLUE tests" ${GLUE_STANDALONE_PROJECT})

add_subdirectory(src)

if (GLUE_ENABLE_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

if(NOT GLUE_ENABLE_INSTALL)
  return()
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY ExactVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/glueConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Includes
install(
  DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
