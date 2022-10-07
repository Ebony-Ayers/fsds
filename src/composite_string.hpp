#ifndef COMPOSITE_STRING_HPP_HEADER_GUARD
#define COMPOSITE_STRING_HPP_HEADER_GUARD
#include "fsds_options.hpp"

#include <iostream>
#include "static_string.hpp"
#include "dynamic_string.hpp"

namespace fsds
{
	class CompositeStringItterator;
	class StaticString;
	class DynamicString;
	
	class CompositeString
	{
		public:
			using CharT = uint8_t;
			using WideCharT = uint64_t;

			static constexpr size_t npos = size_t(-1);

			CompositeString(const CharT* str, const size_t& size);
			CompositeString(const CompositeString& other) noexcept;
			explicit CompositeString(const StaticString& other);
			explicit CompositeString(const DynamicString& other);
			
			//container methods
			const CompositeString operator[](size_t pos) const;

			constexpr size_t size() const;
			constexpr bool isEmpty() const;
			
			const CharT* render() const;
			const char* cstr() const;

			bool valueEquality(const CompositeString& other) const;
			bool referenceEquality(const CompositeString& other) const;
			bool valueEquality(const StaticString& other) const;
			bool referenceEquality(const StaticString& other) const;
			bool valueEquality(const DynamicString& other) const;
			bool referenceEquality(const DynamicString& other) const;

			//string methods
			CompositeString substr(const size_t& pos, const size_t& len) const;

			CompositeString& append(const CompositeString& str);
			CompositeString& insert(size_t pos, const CompositeString& str);
			CompositeString& replace(size_t start, size_t end, const CompositeString& str);
			CompositeString& replace(const CompositeString& oldStr, const CompositeString& newStr);
			
			int compare(const CompositeString& other);
			bool contains(const CompositeString& str);

			size_t find(const CompositeString& str) const;
			CompositeStringItterator findItterator(const CompositeString& str) const;
			size_t findFirstCharacter(const CompositeString& str) const;
			CompositeStringItterator findFirstCharacterItterator(const CompositeString& str) const;

			bool startsWith(const CompositeString& str) const;
			bool endsWith(const CompositeString& str) const;

			size_t numCodePointsInFirstCharacter(size_t codePointOffset = 0) const;
		
		private:
			bool firstCharacterEquality(const CompositeString& str) const;
			struct firstCharacterEqualityWithLengthReturnType { bool result; size_t numCodePoints; };
			CompositeString::firstCharacterEqualityWithLengthReturnType firstCharacterEqualityWithLength(const CompositeString& str) const;

			StaticString* m_staticStrs;
			DynamicString* m_dynamicStrs;
			size_t m_numStaticStrs;
			size_t m_numDynamicStrs;
			size_t m_capacityStaticStrs;
			size_t m_capacityDynamicStrs;
			const size_t m_size;
	};

	bool operator==(const CompositeString& lhs, const CompositeString& rhs);
	bool operator!=(const CompositeString& lhs, const CompositeString& rhs);
	bool operator==(const CompositeString& lhs, const StaticString& rhs);
	bool operator!=(const CompositeString& lhs, const StaticString& rhs);
	bool operator==(const StaticString& lhs, const CompositeString& rhs);
	bool operator!=(const StaticString& lhs, const CompositeString& rhs);
	bool operator==(const CompositeString& lhs, const DynamicString& rhs);
	bool operator!=(const CompositeString& lhs, const DynamicString& rhs);
	bool operator==(const DynamicString& lhs, const CompositeString& rhs);
	bool operator!=(const DynamicString& lhs, const CompositeString& rhs);

	std::ostream& operator<<(std::ostream& os, const CompositeString& str);

	class CompositeStringItterator
	{
		public:
			CompositeStringItterator(const CompositeString* sStr);
			CompositeStringItterator(const CompositeString* sStr, const size_t& pos, const size_t& codePoint);
			CompositeStringItterator(const CompositeString* sStr, const bool& isNPos);

			CompositeString get() const;

			bool next();
			bool previous();

			constexpr bool isNPos() const;
		
		private:
			const CompositeString* m_str;
			size_t m_pos;
			size_t m_chrIndex;
	};
}

#include "composite_string.inl"

#endif //#ifndef COMPOSITE_STRING_HPP_HEADER_GUARD