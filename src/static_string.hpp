#ifndef STATIC_STRING_HPP_HEADER_GUARD
#define STATIC_STRING_HPP_HEADER_GUARD
#include "fsds_options.hpp"

#include <iostream>
#include "utf-8.hpp"
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
			constexpr size_t numCodePointsInString() const;
			constexpr bool isEmpty() const;
			
			constexpr const CharT* data() const;
			const char* cstr() const;

			bool valueEquality(const StaticString& other) const;
			bool valueEquality(const DynamicString& other) const;
			
			//string methods
			StaticString substr(const size_t& pos, const size_t& len) const;
			DynamicString mutableSubstr(const size_t& pos, const size_t& len) const;

			DynamicString concatenate(const StaticString& str) const;
			DynamicString concatenate(const DynamicString& str) const;
			DynamicString concatenateFront(const StaticString& str) const;
			DynamicString concatenateFront(const DynamicString& str) const;
			DynamicString insert(size_t pos, const StaticString& str);
			DynamicString insert(size_t pos, const DynamicString& str);
			DynamicString replace(size_t start, size_t end, const StaticString& str);
			DynamicString replace(size_t start, size_t end, const DynamicString& str);
			DynamicString replace(const StaticString& oldStr, const StaticString& newStr);
			DynamicString replace(const StaticString& oldStr, const DynamicString& newStr);
			DynamicString replace(const DynamicString& oldStr, const StaticString& newStr);
			DynamicString replace(const DynamicString& oldStr, const DynamicString& newStr);
			
			int compare(const StaticString& other) const;
			int compare(const DynamicString& other) const;
			bool contains(const StaticString& str) const;
			bool contains(const DynamicString& str) const;

			size_t find(const StaticString& str) const;
			size_t find(const DynamicString& str) const;
			StaticStringItterator findItterator(const StaticString& str) const;
			StaticStringItterator findItterator(const DynamicString& str) const;
			size_t findAnyCharacter(const StaticString& str) const;
			size_t findAnyCharacter(const DynamicString& str) const;
			StaticStringItterator findAnyCharacterItterator(const StaticString& str) const;
			StaticStringItterator findAnyCharacterItterator(const DynamicString& str) const;
			size_t findNotAnyCharacter(const StaticString& str) const;
			size_t findNotAnyCharacter(const DynamicString& str) const;
			StaticStringItterator findNotAnyCharacterItterator(const StaticString& str) const;
			StaticStringItterator findNotAnyCharacterItterator(const DynamicString& str) const;

			bool startsWith(const StaticString& str) const;
			bool startsWith(const DynamicString& str) const;
			bool endsWith(const StaticString& str) const;
			bool endsWith(const DynamicString& str) const;

			constexpr size_t numCodePointsInFirstCharacter(size_t codePointOffset = 0) const;
		
		private:
			//constexpr bool firstCharacterEquality(const StaticString& str) const;
			struct firstCharacterEqualityWithLengthReturnType { bool result; size_t numCodePoints; };
			constexpr StaticString::firstCharacterEqualityWithLengthReturnType firstCharacterEqualityWithLength(const StaticString& str, size_t codePointOffset = 0) const;

			const CharT* m_str;
			const size_t m_size;

			friend class DynamicString;
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

			constexpr const StaticString* str();
			constexpr size_t currentPosition();
			constexpr size_t currentCodePointOffset(); 

			constexpr bool isNPos() const;
		
		private:
			const StaticString* m_str;
			size_t m_pos;
			size_t m_chrIndex;
	};
}

#include "static_string.inl"

#endif //#ifndef STATIC_STRING_HPP_HEADER_GUARD