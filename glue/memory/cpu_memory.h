#pragma once

#include "memory_allocator.h"
#include <map>
#include <mutex>

namespace cpu_Memory
{
    // Class for memory allocation on CPU
    class MemoryAllocator : public BaseMemoryAllocator
    {
    public:
        MemoryAllocator()
            : m_data( nullptr )
            , m_alignedData( nullptr )
        {}

        static MemoryAllocator & instance()
        {
            static MemoryAllocator allocator;
            return allocator;
        }

        virtual ~MemoryAllocator()
        {
            m_lock.lock();
            _free();
            m_lock.unlock();
        }

        // Returns a pointer to an allocated memory. If memory size of allocated memory chuck is enough for requested size
        // then return a point from preallocated memory, otherwise allocate heap memory
        template <typename _DataType = uint8_t>
        _DataType * allocate( size_t size = 1 )
        {
            m_lock.lock();
            if ( m_data != nullptr ) {
                const size_t overallSize = size * sizeof( _DataType );

                if ( overallSize < m_size ) {
                    const uint8_t level = _getAllocationLevel( overallSize );

                    if ( _split( level ) ) {
                        std::set<size_t>::iterator chunk = m_freeChunk[level].begin();
                        _DataType * address = reinterpret_cast<_DataType *>( m_alignedData + *chunk );
                        m_allocatedChunk.insert( std::pair<size_t, uint8_t>( *chunk, level ) );
                        m_freeChunk[level].erase( chunk );
                        m_lock.unlock();
                        return address;
                    }
                }
            }
            m_lock.unlock();

            // if no space in preallocated memory, allocate as usual memory
            return new _DataType[size];
        }

        // Deallocates a memory by input address. If a pointer points on allocated chuck of memory inside the allocator then
        // the allocator just removes a reference to such area without any cost, otherwise heap allocation
        template <typename _DataType>
        void free( _DataType * address )
        {
            m_lock.lock();
            if ( m_data != nullptr && reinterpret_cast<uint8_t *>( address ) >= m_alignedData ) {
                std::map<size_t, uint8_t>::iterator pos = m_allocatedChunk.find( static_cast<size_t>( reinterpret_cast<uint8_t *>( address ) - m_alignedData ) );

                if ( pos != m_allocatedChunk.end() ) {
                    m_freeChunk[pos->second].insert( pos->first );
                    _merge( pos->first, pos->second );
                    m_allocatedChunk.erase( pos );
                    m_lock.unlock();
                    return;
                }
            }
            m_lock.unlock();

            delete[] address;
        }

    private:
        uint8_t * m_data; // a pointer to memory allocated chunk
        uint8_t * m_alignedData; // aligned pointer for SIMD access
        std::mutex m_lock;

        // a map which holds an information about allocated memory in preallocated memory chunk
        // first parameter is an offset from preallocated memory, second parameter is a power of 2 (level)
        std::map<size_t, uint8_t> m_allocatedChunk;

        // true memory allocation on CPU
        virtual void _allocate( size_t size )
        {
            m_lock.lock();
            if ( m_size != size && size > 0 ) {
                if ( !m_allocatedChunk.empty() )
                    throw std::logic_error( "Cannot free a memory on CPU. Not all objects were previously deallocated from allocator." );

                _free();

                const size_t alignment = 32u; // AVX alignment requirement
                m_data = new uint8_t[size + alignment];
                const std::uintptr_t dataAddress = reinterpret_cast<std::uintptr_t>( m_data );
                m_alignedData = ( ( dataAddress % alignment ) == 0 ) ? m_data : m_data + ( alignment - ( dataAddress % alignment ) );

                m_size = size;
            }
            m_lock.unlock();
        }

        // true memory deallocation on CPU
        virtual void _deallocate()
        {
            if ( m_data != nullptr ) {
                delete[] m_data;
                m_data = nullptr;
                m_alignedData = nullptr;
            }

            m_allocatedChunk.clear();
        }

        MemoryAllocator( const MemoryAllocator & ) {}
        MemoryAllocator & operator=( const MemoryAllocator & )
        {
            return ( *this );
        }
    };
}
