#pragma once
#include "fsds_options.hpp"

#include <iostream>
#include "dynamic_string.hpp"

namespace fsds
{
	class StaticStringItterator;
	class DynamicString;
	
	class StaticString
	{
		public:
			using CharT = uint8_t;
			using WideCharT = uint64_t;

			static constexpr size_t npos = size_t(-1);

			StaticString(const CharT* str, const size_t& size);
			StaticString(const StaticString& other) noexcept;
			
			//container methods
			const StaticString operator[](size_t pos) const;

			constexpr size_t size() const;
			constexpr bool isEmpty() const;
			
			constexpr const CharT* data() const;
			const char* cstr() const;

			bool valueEquality(const StaticString& other) const;
			bool referenceEquality(const StaticString& other) const;
			bool valueEquality(const DynamicString& other) const;
			bool referenceEquality(const DynamicString& other) const;

			//string methods
			StaticString substr(const size_t& pos, const size_t& len) const;
			
			int compare(const StaticString& other);
			bool contains(const StaticString& str);

			size_t find(const StaticString& str) const;
			StaticStringItterator findItterator(const StaticString& str) const;
			size_t findFirstCharacter(const StaticString& str) const;
			StaticStringItterator findFirstCharacterItterator(const StaticString& str) const;

			bool startsWith(const StaticString& str) const;
			bool endsWith(const StaticString& str) const;

			size_t numCodePointsInFirstCharacter(size_t codePointOffset = 0) const;
		
		private:
			bool firstCharacterEquality(const StaticString& str) const;
			struct firstCharacterEqualityWithLengthReturnType { bool result; size_t numCodePoints; };
			StaticString::firstCharacterEqualityWithLengthReturnType firstCharacterEqualityWithLength(const StaticString& str) const;

			const CharT* m_str;
			const size_t m_size;
	};

	bool operator==(const StaticString& lhs, const StaticString& rhs);
	bool operator!=(const StaticString& lhs, const StaticString& rhs);
	bool operator==(const StaticString& lhs, const DynamicString& rhs);
	bool operator!=(const StaticString& lhs, const DynamicString& rhs);

	std::ostream& operator<<(std::ostream& os, const StaticString& str);

	class StaticStringItterator
	{
		public:
			StaticStringItterator(const StaticString* sStr);
			StaticStringItterator(const StaticString* sStr, const size_t& pos, const size_t& codePoint);
			StaticStringItterator(const StaticString* sStr, const bool& isNPos);

			StaticString get() const;

			bool next();
			bool previous();

			constexpr bool isNPos() const;
		
		private:
			const StaticString* m_str;
			size_t m_pos;
			size_t m_chrIndex;
	};
}

#include "static_string.inl"