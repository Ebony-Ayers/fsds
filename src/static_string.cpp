#include "fsds_options.hpp"
#include "static_string.hpp"

#include <iostream>
#include <limits>

namespace fsds
{
	StaticString::StaticString(const CharT* str, const size_t& size)
	: m_str(str), m_size(size)
	{}
	StaticString::StaticString(const StaticString& other) noexcept
	: m_str(other.data()), m_size(other.size())
	{}

	const StaticString StaticString::operator[](size_t pos) const
	{
		if(pos > this->m_size) [[unlikely]]
		{
			throw std::out_of_range("Index out of range");
		}
		else
		{
			return StaticString(this->m_str + fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos), 1);
		}
	}

	const char* StaticString::cstr() const
	{
		return reinterpret_cast<const char*>(this->m_str);
	}

	bool StaticString::valueEquality(const StaticString& other) const
	{
		return fsds::utf8HelperFunctions::equality(this->m_str, this->m_size, other.data(), other.size());
	}
	bool StaticString::valueEquality(const DynamicString& other) const
	{
		return fsds::utf8HelperFunctions::equality(this->m_str, this->m_size, other.data(), other.size());
	}
	
	StaticString StaticString::substr(const size_t& pos, const size_t& len) const
	{
		return StaticString(this->m_str + fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos), len);
	}
	DynamicString StaticString::mutableSubstr(const size_t& pos, const size_t& len) const
	{
		size_t codePointOffsetStart = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos);
		size_t codePointOffsetEnd = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, this->m_size, pos, codePointOffsetStart);
		DynamicString result = DynamicString(len, codePointOffsetEnd - codePointOffsetStart);
		std::copy(this->m_str + codePointOffsetStart, this->m_str + codePointOffsetEnd, result.data());
		return result;
	}

	DynamicString StaticString::concatenate(const StaticString& str) const
	{
		auto numCodePointsInThis = this->numCodePointsInString();
		auto numCodePointsInStr = str.numCodePointsInString();
		DynamicString result = DynamicString(this->m_size + str.size(), numCodePointsInThis + numCodePointsInStr);
		fsds::utf8HelperFunctions::concatenate(this->m_str, numCodePointsInThis, str.data(), numCodePointsInStr, result.data());
		return result;
	}
	DynamicString StaticString::concatenate(const DynamicString& str) const
	{
		auto numCodePointsInThis = this->numCodePointsInString();
		DynamicString result = DynamicString(this->m_size + str.size(), numCodePointsInThis + str.numCodePointsInString());
		fsds::utf8HelperFunctions::concatenate(this->m_str, numCodePointsInThis, str.data(), str.numCodePointsInString(), result.data());
		return result;
	}
	DynamicString StaticString::concatenateFront(const StaticString& str) const
	{
		auto numCodePointsInThis = this->numCodePointsInString();
		auto numCodePointsInStr = str.numCodePointsInString();
		DynamicString result = DynamicString(this->m_size + str.size(), numCodePointsInThis + numCodePointsInStr);
		fsds::utf8HelperFunctions::concatenate(str.data(), numCodePointsInStr, this->m_str, numCodePointsInThis, result.data());
		return result;
	}
	DynamicString StaticString::concatenateFront(const DynamicString& str) const
	{
		auto numCodePointsInThis = this->numCodePointsInString();
		DynamicString result = DynamicString(this->m_size + str.size(), numCodePointsInThis + str.numCodePointsInString());
		fsds::utf8HelperFunctions::concatenate(str.data(), str.numCodePointsInString(), this->m_str, numCodePointsInThis, result.data());
		return result;
	}
	DynamicString StaticString::insert(size_t pos, const StaticString& str)
	{
		auto numCodePointsInThis = this->numCodePointsInString();
		auto numCodePointsInStr = str.numCodePointsInString();
		DynamicString result = DynamicString(this->m_size + str.size(), numCodePointsInThis + numCodePointsInStr);
		fsds::utf8HelperFunctions::insert(this->m_str, fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos), numCodePointsInThis, str.data(), numCodePointsInStr, result.data());
		return result;
	}
	DynamicString StaticString::insert(size_t pos, const DynamicString& str)
	{
		auto numCodePointsInThis = this->numCodePointsInString();
		DynamicString result = DynamicString(this->m_size + str.size(), numCodePointsInThis + str.numCodePointsInString());
		fsds::utf8HelperFunctions::insert(this->m_str, fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos), numCodePointsInThis, str.data(), str.numCodePointsInString(), result.data());
		return result;
	}
	//DynamicString StaticString::replace(size_t start, size_t end, const StaticString& str);
	//DynamicString StaticString::replace(size_t start, size_t end, const DynamicString& str);
	//DynamicString StaticString::replace(const StaticString& oldStr, const StaticString& newStr);
	//DynamicString StaticString::replace(const StaticString& oldStr, const DynamicString& newStr);
	//DynamicString StaticString::replace(const DynamicString& oldStr, const StaticString& newStr);
	//DynamicString StaticString::replace(const DynamicString& oldStr, const DynamicString& newStr);

	int StaticString::compare(const StaticString& other) const
	{
		return fsds::utf8HelperFunctions::compare(this->m_str, this->m_size, other.data(), other.size());
	}
	int StaticString::compare(const DynamicString& other) const
	{
		return fsds::utf8HelperFunctions::compare(this->m_str, this->m_size, other.data(), other.size());
	}
	bool StaticString::contains(const StaticString& str) const
	{
		return find(str) != StaticString::npos;
	}
	bool StaticString::contains(const DynamicString& str) const
	{
		return find(str) != StaticString::npos;
	}

	size_t StaticString::find(const StaticString& str) const
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return StaticString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size()).index;
		}
	}
	size_t StaticString::find(const DynamicString& str) const
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return StaticString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size()).index;
		}
	}
	StaticStringItterator StaticString::findItterator(const StaticString& str) const
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return StaticStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos);
			
			if(result.index == StaticString::npos)
			{
				return StaticStringItterator(this, true);
			}
			else
			{
				return StaticStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	StaticStringItterator StaticString::findItterator(const DynamicString& str) const
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return StaticStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos);
			
			if(result.index == StaticString::npos)
			{
				return StaticStringItterator(this, true);
			}
			else
			{
				return StaticStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
						

	size_t StaticString::findAnyCharacter(const StaticString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos).index;
		}
	}
	size_t StaticString::findAnyCharacter(const DynamicString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos).index;
		}
	}
	StaticStringItterator StaticString::findAnyCharacterItterator(const StaticString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos);
			if(result.index == StaticString::npos)
			{
				return StaticStringItterator(this, true);
			}
			else
			{
				return StaticStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	StaticStringItterator StaticString::findAnyCharacterItterator(const DynamicString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos);
			if(result.index == StaticString::npos)
			{
				return StaticStringItterator(this, true);
			}
			else
			{
				return StaticStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	size_t StaticString::findNotAnyCharacter(const StaticString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos).index;
		}
	}
	size_t StaticString::findNotAnyCharacter(const DynamicString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos).index;
		}
	}
	StaticStringItterator StaticString::findNotAnyCharacterItterator(const StaticString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos);
			if(result.index == StaticString::npos)
			{
				return StaticStringItterator(this, true);
			}
			else
			{
				return StaticStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	StaticStringItterator StaticString::findNotAnyCharacterItterator(const DynamicString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return StaticStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), StaticString::npos);
			if(result.index == StaticString::npos)
			{
				return StaticStringItterator(this, true);
			}
			else
			{
				return StaticStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}

	
	

	bool StaticString::startsWith(const StaticString& str) const
	{
		return fsds::utf8HelperFunctions::startsWith(this->m_str, this->m_size, str.data(), str.size());
	}
	bool StaticString::startsWith(const DynamicString& str) const
	{
		return fsds::utf8HelperFunctions::startsWith(this->m_str, this->m_size, str.data(), str.size());
	}
	bool StaticString::endsWith(const StaticString& str) const
	{
		return fsds::utf8HelperFunctions::endsWith(this->m_str, this->m_size, str.data(), str.size());
	}
	bool StaticString::endsWith(const DynamicString& str) const
	{
		return fsds::utf8HelperFunctions::endsWith(this->m_str, this->m_size, str.data(), str.size());
	}


	bool operator==(const StaticString& lhs, const StaticString& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	bool operator!=(const StaticString& lhs, const StaticString& rhs)
	{
		return !lhs.valueEquality(rhs);
	}
	bool operator==(const StaticString& lhs, const DynamicString& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	bool operator!=(const StaticString& lhs, const DynamicString& rhs)
	{
		return !lhs.valueEquality(rhs);
	}


	std::ostream& operator<<(std::ostream& os, const StaticString& str)
	{
		if(str.size() == 1)
		{
			char cstr[8];

			size_t numCodePoints = str.numCodePointsInFirstCharacter();

			std::copy(str.data(), str.data() + numCodePoints, cstr);
			cstr[numCodePoints] = '\0';

			os << cstr;
		}
		else
		{
			size_t numCodePoints = 0;
			for(size_t i = 0; i < str.size(); i++)
			{
				numCodePoints += str.numCodePointsInFirstCharacter(numCodePoints);
			}

			char* cstr = new char[numCodePoints + 1];
			std::copy(str.data(), str.data() + numCodePoints, cstr);
			cstr[numCodePoints] = '\0';

			os << cstr;

			delete[] cstr;
		}

		return os;
	}


	
	StaticStringItterator::StaticStringItterator(const StaticString* sStr)
	: m_str(sStr), m_pos(0), m_chrIndex(0)
	{}
	StaticStringItterator::StaticStringItterator(const StaticString* sStr, const size_t& pos, const size_t& codePoint)
	: m_str(sStr), m_pos(pos), m_chrIndex(codePoint)
	{}
	StaticStringItterator::StaticStringItterator(const StaticString* sStr, const bool& isNPos)
	: m_str(sStr), m_pos(0), m_chrIndex(0)
	{
		if(isNPos)
		{
			this->m_pos = StaticString::npos;
		}
	}

	StaticString StaticStringItterator::get() const
	{
		return StaticString(m_str->data() + this->m_pos, 1);
	}

	bool StaticStringItterator::next()
	{
		StaticString::CharT codePoint = this->m_str->data()[this->m_pos + 1];
		
		if      ((codePoint & 0b10000000) == 0b00000000) [[likely]] { this->m_pos += 1; }
		else if ((codePoint & 0b11100000) == 0b11000000)            { this->m_pos += 2; }
		else if ((codePoint & 0b11110000) == 0b11100000)            { this->m_pos += 3; }
		else if ((codePoint & 0b11111000) == 0b11110000)            { this->m_pos += 4; }
		else if ((codePoint & 0b11111100) == 0b11111000)            { this->m_pos += 5; }
		else if ((codePoint & 0b11111110) == 0b11111100)            { this->m_pos += 6; }
		else if ((codePoint & 0b11111111) == 0b11111110)            { this->m_pos += 7; }

		this->m_chrIndex++;

		if(this->m_chrIndex > this->m_str->size()) [[unlikely]]
		{
			this->previous();
			return true;
		}
		else
		{
			return false;
		}
	}
	bool StaticStringItterator::previous()
	{
		StaticString::CharT codePoint = this->m_str->data()[this->m_pos - 1];
		
		if      ((codePoint & 0b10000000) == 0b00000000) [[likely]] { this->m_pos -= 1; }
		else if ((codePoint & 0b11100000) == 0b11000000)            { this->m_pos -= 2; }
		else if ((codePoint & 0b11110000) == 0b11100000)            { this->m_pos -= 3; }
		else if ((codePoint & 0b11111000) == 0b11110000)            { this->m_pos -= 4; }
		else if ((codePoint & 0b11111100) == 0b11111000)            { this->m_pos -= 5; }
		else if ((codePoint & 0b11111110) == 0b11111100)            { this->m_pos -= 6; }
		else if ((codePoint & 0b11111111) == 0b11111110)            { this->m_pos -= 7; }

		this->m_chrIndex--;

		//a size_t cannot go below zero so in the event that calling this function goes out of bounds it will overflow
		if(this->m_pos > std::numeric_limits<size_t>::max() - 8) [[unlikely]]
		{
			this->m_pos = 0;
			this->m_chrIndex = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
}