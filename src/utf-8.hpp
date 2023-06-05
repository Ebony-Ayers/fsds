#ifndef UTF8_HEADER_GUARD
#define UTF8_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include <iostream>

namespace fsds
{
	namespace utf8HelperFunctions
	{
		template<typename T>
		constexpr size_t numCodePointsInFirstCharacter(const T* const str, const size_t& codePointOffset = 0, const size_t& npos = size_t(-1));

		template<typename T>
		constexpr size_t codePointOffsetOfCharacterInString(const T* const str, const size_t& size, const size_t& pos, const size_t& posOffset, const size_t& codePointOffset);
		template<typename T>
		constexpr size_t codePointOffsetOfCharacterInString(const T* const str, const size_t& size, const size_t& pos);

		template<typename T>
		constexpr bool equality(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2);

		template<typename T>
		constexpr int compare(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2);

		struct IndexAndOffset{ size_t index; size_t codePointOffset; };
		template<typename T>
		constexpr IndexAndOffset find(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2, const size_t& npos = size_t(-1));
		template<typename T>
		constexpr IndexAndOffset findFirstCharacter(const T* const str1, const size_t& size1, const T* const character, const size_t& npos = size_t(-1));
		template<typename T>
		constexpr IndexAndOffset findAnyCharacter(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2, const size_t& npos = size_t(-1));
		template<typename T>
		constexpr IndexAndOffset findNotAnyCharacter(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2, const size_t& npos = size_t(-1));

		template<typename T>
		constexpr bool startsWith(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2);
		template<typename T>
		constexpr bool endsWith(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2);

		template<typename T>
		constexpr void concatenate(const T* const str1, const size_t& numCodePoints1, const T* const str2, const size_t& numCodePoints2, T* dest);

		template<typename T>
		constexpr void insert(const T* const str1, const size_t& numCodePointsFirst1, const size_t& numCodePointsTotal1, const T* const str2, const size_t& numCodePoints2, T* dest);
		template<typename T>
		constexpr void selfInsertReplace(T* str1, const size_t& numCodePoints1, const T* const str2, const size_t& numCodePoints2, const size_t& blockTwoCodePointStart, const size_t& blockThreeCodePointStart);
	};
}

#include "utf-8.inl"

#endif //#ifndef UTF8_HEADER_GUARD