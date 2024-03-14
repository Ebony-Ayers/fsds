#ifndef ALIGNED_ALLOCATOR_HPP_HEADER_GUARD
#define ALIGNED_ALLOCATOR_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include <memory>

//if the project is being compiled on c++ version less than c++23 manually define the allocation result struct
#if __cplusplus < 202302L
namespace std
{
    template<typename T, typename U>
    struct allocation_result
    {
        T ptr;
        U count;
    };
}
#endif

#ifndef ALLIGNED_RAW_ALLOCATOR_FAKE_TYPE
    #define ALLIGNED_RAW_ALLOCATOR_FAKE_TYPE char
#endif

namespace fsds
{
    template<typename T, size_t alignment>
    class AlignedAllocator
    {
        public:
            constexpr AlignedAllocator() noexcept;

            [[nodiscard]] constexpr T* allocate(size_t n);
            [[nodiscard]] constexpr std::allocation_result<T*, size_t> allocate_at_least(size_t n);
            constexpr void deallocate(T* p, size_t n);
    };

    template<size_t alignment>
    class AlignedRawAllocator
    {
        public:
            constexpr AlignedRawAllocator() noexcept;

            [[nodiscard]] constexpr void* allocate(size_t n);
            [[nodiscard]] constexpr std::allocation_result<void*, size_t> allocate_at_least(size_t n);
            constexpr void deallocate(void* p, size_t n);
        
        private:
            AlignedAllocator<ALLIGNED_RAW_ALLOCATOR_FAKE_TYPE, alignment> m_allocator;
    };
}

#include "aligned_allocator.inl"

#endif //#ifndef ALIGNED_ALLOCATOR_HPP_HEADER_GUARD