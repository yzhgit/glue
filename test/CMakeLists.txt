#
# Copyright 2015 zhangyao<mosee.gd@163.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(source_files
  src/main.cpp

  src/base/barrier_test.cpp

  src/base/tribool_test.cpp
  src/base/expected_test.cpp
  src/base/optional_test.cpp

  src/base/file_mapping_test.cpp
  src/base/object_pool_test.cpp

  src/crypto/aes_test.cpp
  src/crypto/base64_test.cpp
  src/crypto/hex_test.cpp
  src/crypto/md5_test.cpp
  src/crypto/salsa20_test.cpp
  src/crypto/sha256_test.cpp
  src/crypto/sha512_test.cpp
)

add_executable(${PROJECT_NAME}_test
  ${source_files}
)

target_include_directories(${PROJECT_NAME}_test
  PRIVATE
    ${PROJECT_SOURCE_DIR}/thirdparty
)

target_link_libraries(${PROJECT_NAME}_test
  PRIVATE
    glue::base
    glue::crypto
)

##############################################################################
# CTest
##############################################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Catch2")

include(Catch)
catch_discover_tests(${PROJECT_NAME}_test)
