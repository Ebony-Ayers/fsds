#ifndef INLINE_CONTROL_BLOCK_HPP_HEADER_GUARD
#define INLINE_CONTROL_BLOCK_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#if defined(FSDS_COMPILER_CLANG)
    #define ALIGN_MEMBER_PREFIX(n)
    #define ALIGN_MEMBER_POSTFIX(n) __attribute__((aligned(n)))
#elif defined(FSDS_COMPILER_GCC)
    #define ALIGN_MEMBER_PREFIX(n)
    #define ALIGN_MEMBER_POSTFIX(n) __attribute__((aligned(n)))
#elif defined(FSDS_COMPILER_MSVC)
    #define ALIGN_MEMBER_PREFIX(n) __declspec(align(n))
    #define ALIGN_MEMBER_POSTFIX(n)
#endif

namespace fsds
{
    template<typename ControlBlockT, typename DataT, size_t minimalAlignmentOfData = 0>
    struct inlineControlBlock
    {
        public:
            ControlBlockT controlBlock;
            #if(minimalAlignmentOfData == 0)
                DataT data[0]
            #else
                ALIGN_MEMBER_PREFIX(minimalAlignmentOfData) DataT data[0] ALIGN_MEMBER_POSTFIX(minimalAlignmentOfData);
            #endif

            //returns the number of bytes required to allocate the control block and data based on the number of elements in the data
            constexpr size_t requiredAlocationSize(const size_t& n) const
            {
                return sizeof(inlineControlBlock<ControlBlockT, DataT>) + (n * sizeof(DataT));
            }
            static constexpr size_t minimalAlignment()
            {
                return minimalAlignmentOfData;
            }
    };
}

#endif //#ifndef INLINE_CONTROL_BLOCK_HPP_HEADER_GUARD