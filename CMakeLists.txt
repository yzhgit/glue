#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.17.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

project(glue
  VERSION 0.1.0
  LANGUAGES CXX
)

# include(CTest)
# enable_testing()

# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

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
option(GLUE_INSTALL "Generate and install GLUE target" ${GLUE_STANDALONE_PROJECT})
option(GLUE_TEST "Build and perform GLUE tests" ${GLUE_STANDALONE_PROJECT})

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(src)

if(GLUE_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(EXPORT ${PROJECT_NAME}Targets
    NAMESPACE glue::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  )

  configure_package_config_file(
    cmake/glueConfig.cmake.in
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  )
  install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  )

  if(glue_VERSION)
    write_basic_package_version_file(
      "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
      COMPATIBILITY ExactVersion
    )

    install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    )
  endif()

  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if (GLUE_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()
