#ifndef ALIGNED_ALLOCATOR_HPP_HEADER_GUARD
#define ALIGNED_ALLOCATOR_HPP_HEADER_GUARD
#include "fsds_options.hpp"

#include <memory>

namespace fsds
{
    template<typename T, size_t alignment>
    class AlignedAllocator
    {
        public:
            constexpr AlignedAllocator() noexcept;
            constexpr AlignedAllocator(const allocator& other) noexcept;

            [[nodiscard]] constexpr T* allocate(size_t n);
            [[nodiscard]] constexpr std::allocation_result<T*, size_t> allocate_at_least(size_t n);
            constexpr void deallocate(T* p, size_t n);
    };
}

#include "aligned_allocator.inl"

#endif //#ifndef ALIGNED_ALLOCATOR_HPP_HEADER_GUARD