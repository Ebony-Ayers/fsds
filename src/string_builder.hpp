#ifndef STRING_BUILDER_HPP_HEADER_GUARD
#define STRING_BUILDER_HPP_HEADER_GUARD
#include "fsds_options.hpp"
#include "pch.hpp"

#include <iostream>
#include <algorithm>
#include "utf-8.hpp"
#include "static_string.hpp"
#include "dynamic_string.hpp"
#include "seperate_data_list.hpp"

namespace fsds
{
	template<typename SizeType = size_t>
	class StringBuilder
	{
		public:
			static_assert(std::is_same<StaticString::CharT, DynamicString::CharT>::value, "StaticString and DynamicString must use the same character type");
			using CharT = StaticString::CharT;

			static constexpr size_t npos = size_t(-1);
			
			StringBuilder();
			StringBuilder(const StringBuilder& other);
			StringBuilder(StringBuilder&& other);
			~StringBuilder();

			constexpr size_t size() const;
			constexpr size_t numCodePointsInString() const;
			constexpr bool isEmpty() const;

			StringBuilder& concatenate(const DynamicString& str);
			StringBuilder& concatenate(const StaticString& str);
			StringBuilder& concatenateFront(const DynamicString& str);
			StringBuilder& concatenateFront(const StaticString& str);
			StringBuilder& insert(size_t pos, const DynamicString& str);
			StringBuilder& insert(size_t pos, const StaticString& str);
			StringBuilder& replace(size_t start, size_t end, const DynamicString& str);
			StringBuilder& replace(size_t start, size_t end, const StaticString& str);
			StringBuilder& replace(const DynamicString& oldStr, const DynamicString& newStr);
			StringBuilder& replace(const DynamicString& oldStr, const StaticString& newStr);
			StringBuilder& replace(const StaticString& oldStr, const DynamicString& newStr);
			StringBuilder& replace(const StaticString& oldStr, const StaticString& newStr);

			StaticString render();

		private:
			struct PartialString
			{
				const CharT* str;
				SizeType size;
				SizeType numCodePoints;
			};
			
			constexpr PartialString copyInString(const StaticString& str);
			constexpr PartialString copyInString(const DynamicString& str);

			//elementary operations
			constexpr SizeType findPartialStringIndex(const SizeType& pos) const;
			constexpr void appendPartialString(const SizeType& rawStringIndex);
			constexpr void prependPartialString(const SizeType& rawStringIndex);
			constexpr void splitPartialString(const SizeType& partialStringIndex, const SizeType& pos);
			constexpr void insertBeforePartialString(const PartialString&, const SizeType& partialStringIndex);
			constexpr void removePartialString(const SizeType& partialStringIndex);

			fsds::SeperateDataList<PartialString> m_partialStringTable;

			fsds::SeperateDataList<const StaticString*> m_staticStrings;
			fsds::SeperateDataList<DynamicString> m_dynamicStrings;

			SizeType m_size;
			SizeType m_numCodePoints;
	};
}

#include "string_builder.inl"

#endif //#ifndef STRING_BUILDER_HPP_HEADER_GUARD