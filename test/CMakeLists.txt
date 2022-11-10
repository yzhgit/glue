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

include_directories("${CMAKE_SOURCE_DIR}/include/glue/base")
include_directories("${CMAKE_SOURCE_DIR}/thirdparty")

set(source_files
  src/main.cpp

  src/base/result.test.cpp
  src/base/result.constexpr.test.cpp
  src/base/result.failure.test.cpp

  src/base/object_pool.test.cpp

  # src/crypto/aes.test.cpp
  # src/crypto/base64.test.cpp
  # src/crypto/md5.test.cpp
)

add_executable(${PROJECT_NAME}_test
  ${source_files}
)

##############################################################################
# CTest
##############################################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Catch2")

include(Catch)
catch_discover_tests(${PROJECT_NAME}_test)
