namespace fsds
{
    template<typename T, size_t alignment>
    constexpr AlignedAllocator<T, alignment>::AlignedAllocator() noexcept
    {
        //
    }

    template<typename T, size_t alignment>
    [[nodiscard]] constexpr T* AlignedAllocator<T, alignment>::allocate(size_t n)
    {
        auto res = std::aligned_alloc(alignment, n*sizeof(T));
        #ifdef ALLIGNED_ALLOCATOR_PRINT_ALLOCATIONS
            std::cout << "allocating " << n << " bytes at 0x" << std::hex << reinterpret_cast<uint64_t>(res) << std::dec << std::endl; 
        #endif
        return res;
    }

    template<typename T, size_t alignment>
    [[nodiscard]] constexpr std::allocation_result<T*, size_t> AlignedAllocator<T, alignment>::allocate_at_least(size_t n)
    {
        std::allocation_result<T*, size_t> res;
        res.ptr = reinterpret_cast<T*>(std::aligned_alloc(alignment, n*sizeof(T)));
        res.count = n;
        #ifdef ALLIGNED_ALLOCATOR_PRINT_ALLOCATIONS
            std::cout << "allocating " << n << " bytes at 0x" << std::hex << reinterpret_cast<uint64_t>(res.ptr) << std::dec << std::endl; 
        #endif
        return res;
    }

    template<typename T, size_t alignment>
    constexpr void AlignedAllocator<T, alignment>::deallocate(T* p, size_t /*n*/)
    {
        #ifdef ALLIGNED_ALLOCATOR_PRINT_ALLOCATIONS
            std::cout << "deallocating 0x" << std::hex << reinterpret_cast<uint64_t>(p) << std::dec << std::endl; 
        #endif
        std::free(p);
    }



    template<size_t alignment>
    constexpr AlignedRawAllocator<alignment>::AlignedRawAllocator() noexcept
    : m_allocator()
    {
        //
    }

    template<size_t alignment>
    [[nodiscard]] constexpr void* AlignedRawAllocator<alignment>::allocate(size_t n)
    {
        auto res = this->m_allocator.allocate(n);
        #ifdef ALLIGNED_RAW_ALLOCATOR_PRINT_ALLOCATIONS
            std::cout << "allocating " << n << " bytes at 0x" << std::hex << reinterpret_cast<uint64_t>(res) << std::dec << std::endl; 
        #endif
        return reinterpret_cast<void*>(res);
    }

    template<size_t alignment>
    [[nodiscard]] constexpr std::allocation_result<void*, size_t> AlignedRawAllocator<alignment>::allocate_at_least(size_t n)
    {
        std::allocation_result<ALLIGNED_RAW_ALLOCATOR_FAKE_TYPE*, size_t> res = this->m_allocator.allocate_at_least(n);
        std::allocation_result<void*, size_t> ret;
        ret.ptr = reinterpret_cast<void*>(res.ptr);
        ret.count = res.count;
        #ifdef ALLIGNED_RAW_ALLOCATOR_PRINT_ALLOCATIONS
            std::cout << "allocating " << n << " bytes at 0x" << std::hex << reinterpret_cast<uint64_t>(res.ptr) << std::dec << std::endl; 
        #endif
        return ret;
    }

    template<size_t alignment>
    constexpr void AlignedRawAllocator<alignment>::deallocate(void* p, size_t n)
    {
        #ifdef ALLIGNED_RAW_ALLOCATOR_PRINT_ALLOCATIONS
            std::cout << "deallocating " << n << " bytes at 0x" << std::hex << reinterpret_cast<uint64_t>(p) << std::dec << std::endl; 
        #endif
        this->m_allocator.deallocate(reinterpret_cast<ALLIGNED_RAW_ALLOCATOR_FAKE_TYPE*>(p), n);
    }
}