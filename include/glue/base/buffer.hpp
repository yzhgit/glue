//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/common.hpp"

#include "glue/base/exception.hpp"

namespace glue {

template <class T>
class Buffer
/// A buffer class that allocates a buffer of a given type and size
/// in the constructor and deallocates the buffer in the destructor.
///
/// This class is useful everywhere where a temporary buffer
/// is needed.
{
public:
    Buffer(std::size_t length) : m_capacity(length), m_used(length), m_ptr(0), m_ownMem(true)
    /// Creates and allocates the Buffer.
    {
        if (length > 0) { m_ptr = new T[length]; }
    }

    Buffer(T* pMem, std::size_t length)
        : m_capacity(length), m_used(length), m_ptr(pMem), m_ownMem(false)
    /// Creates the Buffer. Length argument specifies the length
    /// of the supplied memory pointed to by pMem in the number
    /// of elements of type T. Supplied pointer is considered
    /// blank and not owned by Buffer, so in this case Buffer
    /// only acts as a wrapper around externally supplied
    /// (and lifetime-managed) memory.
    {}

    Buffer(const T* pMem, std::size_t length)
        : m_capacity(length), m_used(length), m_ptr(0), m_ownMem(true)
    /// Creates and allocates the Buffer; copies the contents of
    /// the supplied memory into the buffer. Length argument specifies
    /// the length of the supplied memory pointed to by pMem in the
    /// number of elements of type T.
    {
        if (m_capacity > 0)
        {
            m_ptr = new T[m_capacity];
            std::memcpy(m_ptr, pMem, m_used * sizeof(T));
        }
    }

    Buffer(const Buffer& other)
        : /// Copy constructor.
        m_capacity(other.m_used)
        , m_used(other.m_used)
        , m_ptr(0)
        , m_ownMem(true)
    {
        if (m_used)
        {
            m_ptr = new T[m_used];
            std::memcpy(m_ptr, other.m_ptr, m_used * sizeof(T));
        }
    }

    Buffer(Buffer&& other) noexcept
        : /// Move constructor.
        m_capacity(other.m_capacity)
        , m_used(other.m_used)
        , m_ptr(other.m_ptr)
        , m_ownMem(other.m_ownMem)
    {
        other.m_capacity = 0;
        other.m_used = 0;
        other.m_ownMem = false;
        other.m_ptr = nullptr;
    }

    Buffer& operator=(const Buffer& other)
    /// Assignment operator.
    {
        if (this != &other)
        {
            Buffer tmp(other);
            swap(tmp);
        }

        return *this;
    }

    Buffer& operator=(Buffer&& other) noexcept
    /// Move assignment operator.
    {
        if (m_ownMem) delete[] m_ptr;

        m_capacity = other.m_capacity;
        m_used = other.m_used;
        m_ptr = other.m_ptr;
        m_ownMem = other.m_ownMem;

        other.m_capacity = 0;
        other.m_used = 0;
        other.m_ownMem = false;
        other.m_ptr = nullptr;

        return *this;
    }

    ~Buffer()
    /// Destroys the Buffer.
    {
        if (m_ownMem) delete[] m_ptr;
    }

    void resize(std::size_t newCapacity, bool preserveContent = true)
    /// Resizes the buffer capacity and size. If preserveContent is true,
    /// the content of the old buffer is copied over to the
    /// new buffer. The new capacity can be larger or smaller than
    /// the current one; if it is smaller, capacity will remain intact.
    /// Size will always be set to the new capacity.
    ///
    /// Buffers only wrapping externally owned storage can not be
    /// resized. If resize is attempted on those, IllegalAccessException
    /// is thrown.
    {
        if (!m_ownMem)
            throw glue::InvalidAccessException(
                "Cannot resize buffer which does not own its storage.");

        if (newCapacity > m_capacity)
        {
            T* ptr = new T[newCapacity];
            if (preserveContent && m_ptr) { std::memcpy(ptr, m_ptr, m_used * sizeof(T)); }
            delete[] m_ptr;
            m_ptr = ptr;
            m_capacity = newCapacity;
        }

        m_used = newCapacity;
    }

    void setCapacity(std::size_t newCapacity, bool preserveContent = true)
    /// Sets the buffer capacity. If preserveContent is true,
    /// the content of the old buffer is copied over to the
    /// new buffer. The new capacity can be larger or smaller than
    /// the current one; size will be set to the new capacity only if
    /// new capacity is smaller than the current size, otherwise it will
    /// remain intact.
    ///
    /// Buffers only wrapping externally owned storage can not be
    /// resized. If resize is attempted on those, IllegalAccessException
    /// is thrown.
    {
        if (!m_ownMem)
            throw glue::InvalidAccessException(
                "Cannot resize buffer which does not own its storage.");

        if (newCapacity != m_capacity)
        {
            T* ptr = 0;
            if (newCapacity > 0)
            {
                ptr = new T[newCapacity];
                if (preserveContent && m_ptr)
                {
                    std::size_t newSz = m_used < newCapacity ? m_used : newCapacity;
                    std::memcpy(ptr, m_ptr, newSz * sizeof(T));
                }
            }
            delete[] m_ptr;
            m_ptr = ptr;
            m_capacity = newCapacity;

            if (newCapacity < m_used) m_used = newCapacity;
        }
    }

    void assign(const T* buf, std::size_t sz)
    /// Assigns the argument buffer to this buffer.
    /// If necessary, resizes the buffer.
    {
        if (0 == sz) return;
        if (sz > m_capacity) resize(sz, false);
        std::memcpy(m_ptr, buf, sz * sizeof(T));
        m_used = sz;
    }

    void append(const T* buf, std::size_t sz)
    /// Resizes this buffer and appends the argument buffer.
    {
        if (0 == sz) return;
        resize(m_used + sz, true);
        std::memcpy(m_ptr + m_used - sz, buf, sz * sizeof(T));
    }

    void append(T val)
    /// Resizes this buffer by one element and appends the argument value.
    {
        resize(m_used + 1, true);
        m_ptr[m_used - 1] = val;
    }

    void append(const Buffer& buf)
    /// Resizes this buffer and appends the argument buffer.
    {
        append(buf.begin(), buf.size());
    }

    std::size_t capacity() const
    /// Returns the allocated memory size in elements.
    {
        return m_capacity;
    }

    std::size_t capacityBytes() const
    /// Returns the allocated memory size in bytes.
    {
        return m_capacity * sizeof(T);
    }

    void swap(Buffer& other)
    /// Swaps the buffer with another one.
    {
        using std::swap;

        swap(m_ptr, other.m_ptr);
        swap(m_capacity, other.m_capacity);
        swap(m_used, other.m_used);
        swap(m_ownMem, other.m_ownMem);
    }

    bool operator==(const Buffer& other) const
    /// Compare operator.
    {
        if (this != &other)
        {
            if (m_used == other.m_used)
            {
                if (m_ptr && other.m_ptr &&
                    std::memcmp(m_ptr, other.m_ptr, m_used * sizeof(T)) == 0)
                {
                    return true;
                }
                else
                    return m_used == 0;
            }
            return false;
        }

        return true;
    }

    bool operator!=(const Buffer& other) const
    /// Compare operator.
    {
        return !(*this == other);
    }

    void clear()
    /// Sets the contents of the buffer to zero.
    {
        std::memset(m_ptr, 0, m_used * sizeof(T));
    }

    std::size_t size() const
    /// Returns the used size of the buffer in elements.
    {
        return m_used;
    }

    std::size_t sizeBytes() const
    /// Returns the used size of the buffer in bytes.
    {
        return m_used * sizeof(T);
    }

    T* begin()
    /// Returns a pointer to the beginning of the buffer.
    {
        return m_ptr;
    }

    const T* begin() const
    /// Returns a pointer to the beginning of the buffer.
    {
        return m_ptr;
    }

    T* end()
    /// Returns a pointer to end of the buffer.
    {
        return m_ptr + m_used;
    }

    const T* end() const
    /// Returns a pointer to the end of the buffer.
    {
        return m_ptr + m_used;
    }

    bool empty() const
    /// Return true if buffer is empty.
    {
        return 0 == m_used;
    }

    T& operator[](std::size_t index)
    {
        GLUE_ASSERT(index < m_used);

        return m_ptr[index];
    }

    const T& operator[](std::size_t index) const
    {
        GLUE_ASSERT(index < m_used);

        return m_ptr[index];
    }

private:
    Buffer();

    std::size_t m_capacity;
    std::size_t m_used;
    T* m_ptr;
    bool m_ownMem;
};

} // namespace glue
