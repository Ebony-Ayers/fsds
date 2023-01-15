#ifndef DYNAMIC_STRING_HPP_HEADER_GUARD
#define DYNAMIC_STRING_HPP_HEADER_GUARD
#include "fsds_options.hpp"

#include <iostream>
#include "static_string.hpp"

namespace fsds
{
	class DynamicStringItterator;
	class StaticString;
	
	class DynamicString
	{
		public:
			using CharT = uint8_t;
			using WideCharT = uint64_t;

			static constexpr size_t npos = size_t(-1);

			DynamicString(const CharT* str, const size_t& size);
			DynamicString(const CharT* str, const size_t& size, const size_t& numCodePoints);
			DynamicString(const size_t& size, const size_t& numCodePoints);
			DynamicString(const DynamicString& other) noexcept;
			~DynamicString();

			operator StaticString() const;
			
			//container methods
			const DynamicString operator[](size_t pos) const;
			void set(size_t pos, const DynamicString& character);

			constexpr size_t size() const;
			constexpr size_t capacity() const;
			constexpr size_t numCodePointsInString() const;
			constexpr bool isEmpty() const;
			
			constexpr CharT* data() const;
			const char* cstr() const;

			bool valueEquality(const DynamicString& other) const;
			bool valueEquality(const StaticString& other) const;
			
			//string methods
			DynamicString substr(const size_t& pos, const size_t& len) const;
			StaticString constSubstr(const size_t& pos, const size_t& len) const;

			DynamicString& concatenate(const DynamicString& str);
			DynamicString& concatenate(const StaticString& str);
			DynamicString& concatenateFront(const DynamicString& str);
			DynamicString& concatenateFront(const StaticString& str);
			DynamicString& insert(size_t pos, const DynamicString& str);
			DynamicString& insert(size_t pos, const StaticString& str);
			DynamicString& replace(size_t start, size_t end, const DynamicString& str);
			DynamicString& replace(size_t start, size_t end, const StaticString& str);
			DynamicString& replace(const DynamicString& oldStr, const DynamicString& newStr);
			DynamicString& replace(const DynamicString& oldStr, const StaticString& newStr);
			DynamicString& replace(const StaticString& oldStr, const DynamicString& newStr);
			DynamicString& replace(const StaticString& oldStr, const StaticString& newStr);
			
			int compare(const DynamicString& other) const;
			int compare(const StaticString& other) const;
			bool contains(const DynamicString& str) const;
			bool contains(const StaticString& str) const;

			size_t find(const DynamicString& str) const;
			size_t find(const StaticString& str) const;
			DynamicStringItterator findItterator(const DynamicString& str);
			DynamicStringItterator findItterator(const StaticString& str);
			size_t findFirstCharacter(const DynamicString& str) const;
			size_t findFirstCharacter(const StaticString& str) const;
			DynamicStringItterator findFirstCharacterItterator(const DynamicString& str);
			DynamicStringItterator findFirstCharacterItterator(const StaticString& str);

			bool startsWith(const DynamicString& str) const;
			bool startsWith(const StaticString& str) const;
			bool endsWith(const DynamicString& str) const;
			bool endsWith(const StaticString& str) const;

			constexpr size_t numCodePointsInFirstCharacter(size_t codePointOffset = 0) const;
		
		private:
			struct firstCharacterEqualityWithLengthReturnType { bool result; size_t numCodePoints; };
			constexpr bool firstCharacterEquality(const DynamicString& str) const;
			constexpr DynamicString::firstCharacterEqualityWithLengthReturnType firstCharacterEqualityWithLength(const DynamicString& str) const;
			bool firstCharacterEquality(const StaticString& str) const;
			DynamicString::firstCharacterEqualityWithLengthReturnType firstCharacterEqualityWithLength(const StaticString& str) const;
			constexpr void reallocate(size_t newSize);

			CharT* m_str;
			size_t m_size;
			size_t m_capacity;
			size_t m_numCodePoints;
	};

	bool operator==(const DynamicString& lhs, const DynamicString& rhs);
	bool operator!=(const DynamicString& lhs, const DynamicString& rhs);
	bool operator==(const DynamicString& lhs, const StaticString& rhs);
	bool operator!=(const DynamicString& lhs, const StaticString& rhs);

	std::ostream& operator<<(std::ostream& os, const DynamicString& str);

	class DynamicStringItterator
	{
		public:
			DynamicStringItterator(DynamicString* sStr);
			DynamicStringItterator(DynamicString* sStr, const size_t& pos, const size_t& codePoint);
			DynamicStringItterator(DynamicString* sStr, const bool& isNPos);

			DynamicString get() const;
			void set(const DynamicString& character);

			bool next();
			bool previous();

			constexpr bool isNPos() const;
		
		private:
			DynamicString* m_str;
			size_t m_pos;
			size_t m_chrIndex;
	};
}


#include "dynamic_string.inl"

#endif //#ifndef DYNAMIC_STRING_HPP_HEADER_GUARD