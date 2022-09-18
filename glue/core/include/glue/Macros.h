/*
 * Copyright 2015 zhangyao<mosee.gd@163.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define CHECK_ALIGN(ptr, alignment)                                            \
    do {                                                                       \
        constexpr size_t status =                                              \
            reinterpret_cast<uintptr_t>(ptr) % alignment;                      \
        static_assert(status == 0, "ptr must be aligned");                     \
    } while (0)

/* Offset of member MEMBER in a struct of type TYPE. */
#define offsetof(OBJECT, MEMBER) __builtin_offsetof(OBJECT, MEMBER)

#if __cplusplus > 201103L
    #define GL_DEPRECATED(reason) [[deprecated(reason)]]
#elif defined(__clang__)
    #define GL_DEPRECATED(reason) __attribute__((deprecated(reason)))
#elif defined(__GNUG__)
    #define GL_DEPRECATED(reason) __attribute__((deprecated))
#elif defined(_MSC_VER)
    #if _MSC_VER < 1910
        #define GL_DEPRECATED(reason) __declspec(deprecated)
    #else
        #define GL_DEPRECATED(reason) [[deprecated(reason)]]
    #endif
#endif
