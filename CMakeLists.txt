#
# Copyright © 2017 zhangyao. All rights reserved.
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.17.0)
project(glue VERSION 0.1.0)

# include(CTest)
# enable_testing()

# Output directory is correct by default for most build setups. However, when
# building Falcon as a DLL, it is important to have the DLL in the same
# directory as the executable using it. Thus, we put all executables in a single
# /bin directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# when absl is included as subproject (i.e. using add_subdirectory(glue))
# in the source tree of a project that uses it, install rules are disabled.
if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  option(GL_ENABLE_INSTALL "Enable install rule" OFF)
else()
  option(GL_ENABLE_INSTALL "Enable install rule" ON)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(glue)

# if(GL_ENABLE_INSTALL)
#   # We don't support system-wide installation
#   list(APPEND SYSTEM_INSTALL_DIRS "/usr/local" "/usr" "/opt/" "/opt/local" "c:/Program Files/${PROJECT_NAME}")
#   if(NOT DEFINED CMAKE_INSTALL_PREFIX OR CMAKE_INSTALL_PREFIX IN_LIST SYSTEM_INSTALL_DIRS)
#     message(WARNING "\
#   The default and system-level install directories are unsupported except in LTS \
#   releases of Abseil.  Please set CMAKE_INSTALL_PREFIX to install Abseil in your \
#   source or build tree directly.\
#     ")
#   endif()

#   # install as a subdirectory only
#   install(EXPORT ${PROJECT_NAME}Targets
#     NAMESPACE gl::
#     DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
#   )

#   configure_package_config_file(
#     CMake/glueConfig.cmake.in
#     "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#     INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
#   )
#   install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#     DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
#   )

#   # Abseil only has a version in LTS releases.  This mechanism is accomplished
#   # Abseil's internal Copybara (https://github.com/google/copybara) workflows and
#   # isn't visible in the CMake buildsystem itself.
#   if(glue_VERSION)
#     write_basic_package_version_file(
#       "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
#       COMPATIBILITY ExactVersion
#     )

#     install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
#       DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
#     )
#   endif()  # glue_VERSION

#   install(DIRECTORY glue
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#     FILES_MATCHING
#       PATTERN "*.inc"
#       PATTERN "*.h"
#       PATTERN "copts" EXCLUDE
#     )
# endif()  # GL_ENABLE_INSTALL

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
