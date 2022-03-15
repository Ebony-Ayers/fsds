#pragma once
#include "fsds_options.hpp"
#include <iostream>

namespace fsds
{
	class StaticStringItterator;
	
	class StaticString
	{
		public:
			using CharT = uint8_t;
			using WideCharT = uint64_t;

			static constexpr size_t npos = size_t(-1);

			constexpr StaticString(const CharT* str, const size_t& size);
			constexpr StaticString(const StaticString& other) noexcept;
			
			//container methods
			constexpr const StaticString operator[](size_t pos) const;

			constexpr size_t size() const;
			constexpr bool isEmpty() const;
			
			constexpr const CharT* data() const;
			const char* cstr() const;

			constexpr bool valueEquality(const StaticString& other) const;
			constexpr bool referenceEquality(const StaticString& other) const;

			//string methods
			constexpr StaticString substr(const size_t& pos, const size_t& len) const;
			
			constexpr int compare(const StaticString& other);
			constexpr bool contains(const StaticString& str);

			constexpr size_t find(const StaticString& str) const;
			constexpr StaticStringItterator findItterator(const StaticString& str) const;
			constexpr size_t findFirstCharacter(const StaticString& str) const;
			constexpr StaticStringItterator findFirstCharacterItterator(const StaticString& str) const;

			constexpr bool startsWith(const StaticString& str) const;
			constexpr bool endsWith(const StaticString& str) const;

			constexpr size_t numCodePointsInFirstCharacter(size_t codePointOffset = 0) const;
		
		private:
			constexpr bool firstCharacterEquality(const StaticString& str) const;
			struct firstCharacterEqualityWithLengthReturnType { bool result; size_t numCodePoints; };
			constexpr StaticString::firstCharacterEqualityWithLengthReturnType firstCharacterEqualityWithLength(const StaticString& str) const;

			const CharT* m_str;
			const size_t m_size;
	};

	constexpr bool operator==(const StaticString& lhs, const StaticString& rhs);
	constexpr bool operator!=(const StaticString& lhs, const StaticString& rhs);

	class StaticStringItterator
	{
		public:
			constexpr StaticStringItterator(const StaticString* sStr);
			constexpr StaticStringItterator(const StaticString* sStr, const size_t& pos, const size_t& codePoint);
			constexpr StaticStringItterator(const StaticString* sStr, const bool& isNPos);

			constexpr StaticString get() const;

			constexpr bool next();
			constexpr bool previous();

			constexpr bool isNPos() const;
		
		private:
			const StaticString* m_str;
			size_t m_pos;
			size_t m_chrIndex;
	};

	std::ostream& operator<<(std::ostream& os, const StaticString& str);
}

#include "static_string.inl"