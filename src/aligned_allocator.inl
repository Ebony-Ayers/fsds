namespace fsds
{
    template<typename T, size_t alignment>
    constexpr AlignedAllocator<T, alignment>::AlignedAllocator() noexcept
    {
        //
    }
    template<typename T, size_t alignment>
    constexpr AlignedAllocator<T, alignment>::AlignedAllocator(const allocator& other) noexcept
    {
        //
    }

    template<typename T, size_t alignment>
    [[nodiscard]] constexpr T* AlignedAllocator<T, alignment>::allocate(size_t n)
    {
        if(n % alignment == 0)
        {
            return std::aligned_alloc(alignment, n*sizeof(T));
        }
        else
        {
            size_t numBytesToAllocate = n*sizeof(T);
            size_t numBytesToAllocate = numBytesToAllocate+(alignment-(numBytesToAllocate%alignment));
            return std::aligned_alloc(alignment, numBytesToAllocate);
        }
    }

    template<typename T, size_t alignment>
    [[nodiscard]] constexpr std::allocation_result<T*, size_t> AlignedAllocator<T, alignment>::allocate_at_least(size_t n)
    {
        if(n % alignment == 0)
        {
            std::allocation_result res;
            res.ptr = std::aligned_alloc(alignment, n*sizeof(T));
            res.count = n;
            return res;
        }
        else
        {
            size_t numBytesToAllocate = n*sizeof(T);
            size_t numBytesToAllocate = numBytesToAllocate+(alignment-(numBytesToAllocate%alignment));
            std::allocation_result res;
            res.ptr = std::aligned_alloc(alignment, numBytesToAllocate);
            res.count = amountToAllocate;
            return res;
        }
    }

    template<typename T, size_t alignment>
    constexpr void AlignedAllocator<T, alignment>::deallocate(T* p, size_t /*n*/)
    {
        std::free(p);
    }
}