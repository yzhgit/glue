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

#pragma once

#include <string>

#include "glue/core.h"

namespace glue {

class CORE_API File {
  protected:
    File();
    File(const std::string &path);
    virtual ~File();
    void swap(File &file);
    void setPath(const std::string &path);
    const std::string &getPath() const;
    bool exists() const;
    bool canRead() const;
    bool canWrite() const;
    bool canExecute() const;
    bool isFile() const;
    bool isDirectory() const;
    bool isLink() const;
    bool isDevice() const;
    bool isHidden() const;
    Timestamp created() const;
    Timestamp getLastModified() const;
    void setLastModified(const Timestamp &ts);
    uint64_t getSize() const;
    void setSize(uint64_t size);
    void setWriteable(bool flag = true);
    void setExecutable(bool flag = true);
    void copyTo(const std::string &path, int options = 0) const;
    void renameTo(const std::string &path, int options = 0);
    void linkTo(const std::string &path, int type) const;
    void remove();
    bool createFile();
    bool createDirectory();
    uint64_t totalSpace() const;
    uint64_t usableSpace() const;
    uint64_t freeSpace() const;
    static void handleLastError(const std::string &path);
    static void convertPath(const std::string &utf8Path,
                            std::wstring &utf16Path);

  private:
    std::string _path;
    std::wstring _upath;
};

} // namespace glue
