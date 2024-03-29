#include "fsds_options.hpp"
#include "dynamic_string.hpp"

#include <iostream>
#include <memory>
#include <algorithm>
#include <limits>

namespace fsds
{
	DynamicString::DynamicString(const CharT* str, const size_t& size)
	: m_str(nullptr), m_size(size), m_capacity(0)
	{
		StaticString sString(str, size);

		size_t codePointOffset = 0;
		for(size_t i = 0; i < this->m_size; i++)
		{
			codePointOffset += sString.numCodePointsInFirstCharacter(codePointOffset);
		}
		this->m_capacity = codePointOffset;
		this->m_capacity = codePointOffset;
		
		std::allocator<DynamicString::CharT> alloc;
		this->m_str = alloc.allocate(this->m_capacity);
		
		std::copy(str, str + codePointOffset, this->m_str);
	}
	DynamicString::DynamicString(const CharT* str, const size_t& size, const size_t& numCodePoints)
	: m_str(nullptr), m_size(size), m_capacity(numCodePoints)
	{
		std::allocator<DynamicString::CharT> alloc;
		this->m_str = alloc.allocate(this->m_capacity);

		std::copy(str, str + numCodePoints, this->m_str);
	}
	DynamicString::DynamicString(const size_t& size, const size_t& numCodePoints)
	: m_str(nullptr), m_size(size), m_capacity(numCodePoints)
	{
		std::allocator<DynamicString::CharT> alloc;
		this->m_str = alloc.allocate(this->m_capacity);
	}
	DynamicString::DynamicString(const DynamicString& other) noexcept
	: m_str(other.data()), m_size(other.size()), m_capacity(other.capacity())
	{
		std::allocator<DynamicString::CharT> alloc;
		this->m_str = alloc.allocate(other.capacity());
		
		std::copy(other.data(), other.data() + capacity(), this->m_str);
	}
	DynamicString::~DynamicString()
	{
		std::allocator<DynamicString::CharT> alloc;
		alloc.deallocate(this->m_str, this->m_capacity);
	}

	DynamicString::operator StaticString() const
	{
		return StaticString(this->m_str, this->m_size);
	}

	const DynamicString DynamicString::operator[](size_t pos) const
	{
		if(pos > this->m_size) [[unlikely]]
		{
			throw std::out_of_range("Index out of range");
		}
		else
		{
			
			return DynamicString(this->m_str + fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos), 1);
		}
	}
	void DynamicString::set(size_t pos, const DynamicString& character)
	{
		if(pos > this->m_size) [[unlikely]]
		{
			throw std::out_of_range("Index out of range");
		}
		else
		{
			size_t codePointOffset = 0;
			for(size_t i = 0; i < pos; i++)
			{
				codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
			}
			size_t numCodePoints1 = this->numCodePointsInFirstCharacter(codePointOffset);
			size_t numCodePoints2 = character.numCodePointsInFirstCharacter();
			//if the number of code points won't change do a simple copy
			if(numCodePoints1 == numCodePoints2) [[likely]]
			{
				std::copy(character.data(), character.data() + numCodePoints2, this->m_str + codePointOffset);
			}
			else
			{
				//get required offsets for copying
				size_t nextCharacterPointOffset = codePointOffset + this->numCodePointsInFirstCharacter(codePointOffset);
				size_t remainingLength = codePointOffset;
				for(size_t i = pos; i < this->m_size; i++)
				{
					remainingLength += this->numCodePointsInFirstCharacter(remainingLength);
				}

				//allocate new memory if needs be and update the number of code points
				if(numCodePoints2 > numCodePoints1) [[unlikely]]
				{
					if(remainingLength + (numCodePoints2 - numCodePoints1) >= this->m_capacity) [[unlikely]]
					{
						this->reallocate(this->m_capacity * 2);
					}
					this->m_capacity += numCodePoints2 - numCodePoints1;
				}
				else
				{
					this->m_capacity -= numCodePoints1 - numCodePoints2;
				}
				//copy the original data so there is a gap for the new character
				std::copy(this->m_str + nextCharacterPointOffset, this->m_str + codePointOffset + remainingLength, this->m_str + codePointOffset + numCodePoints2);
				//copy the character into the gap
				std::copy(character.data(), character.data() + numCodePoints2, this->m_str + codePointOffset);
			}
		}
	}

	const char* DynamicString::cstr() const
	{
		return reinterpret_cast<const char*>(this->m_str);
	}

	bool DynamicString::valueEquality(const DynamicString& other) const
	{
		return fsds::utf8HelperFunctions::equality(this->m_str, this->m_size, other.data(), other.size());
	}
	bool DynamicString::valueEquality(const StaticString& other) const
	{
		return fsds::utf8HelperFunctions::equality(this->m_str, this->m_size, other.data(), other.size());
	}

	DynamicString DynamicString::substr(const size_t& pos, const size_t& len) const
	{
		return DynamicString(this->m_str + fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos), len);
	}

	DynamicString& DynamicString::concatenate(const DynamicString& str)
	{
		size_t numCodePointsInThis = this->m_capacity;
		this->reallocate(this->m_capacity + str.numCodePointsInString());

		std::copy(str.data(), str.data() + str.numCodePointsInString(), this->m_str + numCodePointsInThis);
		this->m_size += str.size();
		
		return *this;
	}
	DynamicString& DynamicString::concatenate(const StaticString& str)
	{
		size_t numCodePointsInThis = this->m_capacity;
		size_t nunCodePointsInStr = str.numCodePointsInString();
		this->reallocate(this->m_capacity + nunCodePointsInStr);

		std::copy(str.data(), str.data() + nunCodePointsInStr, this->m_str + numCodePointsInThis);
		this->m_size += str.size();
		
		return *this;
	}
	DynamicString& DynamicString::concatenateFront(const DynamicString& str)
	{
		size_t numCodePointsInThis = this->m_capacity;
		this->reallocate(this->m_capacity + str.numCodePointsInString());

		std::copy(this->m_str, this->m_str + numCodePointsInThis, this->m_str + str.m_capacity);
		std::copy(str.data(), str.data() + str.numCodePointsInString(), this->m_str);
		this->m_size += str.size();
		
		return *this;
	}
	DynamicString& DynamicString::concatenateFront(const StaticString& str)
	{
		size_t numCodePointsInThis = this->m_capacity;
		size_t nunCodePointsInStr = str.numCodePointsInString();
		this->reallocate(this->m_capacity + nunCodePointsInStr);

		std::copy(this->m_str, this->m_str + numCodePointsInThis, this->m_str + nunCodePointsInStr);
		std::copy(str.data(), str.data() + nunCodePointsInStr, this->m_str);
		this->m_size += str.size();
		
		return *this;
	}

	DynamicString& DynamicString::insert(size_t pos, const DynamicString& str)
	{
		const size_t posCodePointOffset= utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos);
		const size_t previousCapacity = this->m_capacity;
		this->reallocate(this->m_capacity + str.numCodePointsInString());
		fsds::utf8HelperFunctions::selfInsertReplace(this->m_str, previousCapacity, str.data(), str.numCodePointsInString(), posCodePointOffset, posCodePointOffset);
		this->m_size += str.size();

		return *this;
	}
	DynamicString& DynamicString::insert(size_t pos, const StaticString& str)
	{
		const size_t posCodePointOffset= utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, pos);
		const size_t nunCodePointsInStr = str.numCodePointsInString();
		const size_t previousCapacity = this->m_capacity;
		this->reallocate(this->m_capacity + nunCodePointsInStr);
		fsds::utf8HelperFunctions::selfInsertReplace(this->m_str, previousCapacity, str.data(), nunCodePointsInStr, posCodePointOffset, posCodePointOffset);
		this->m_size += str.size();

		return *this;
	}

	DynamicString& DynamicString::replace(size_t start, size_t end, const DynamicString& str)
	{
		const size_t codePointOffsetStart = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, start);
		const size_t codePointOffsetEnd = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, end, start, codePointOffsetStart);
		const size_t newCapacity = (this->m_capacity + str.numCodePointsInString()) - (codePointOffsetEnd - codePointOffsetStart);

		const size_t previousCapacity = this->m_capacity;
		if(newCapacity > this->m_capacity)
		{
			this->reallocate(newCapacity);
		}
		fsds::utf8HelperFunctions::selfInsertReplace(this->m_str, previousCapacity, str.data(), str.numCodePointsInString(), codePointOffsetStart, codePointOffsetEnd);
		this->m_size += str.size();
		this->m_size -= end - start;
		if(newCapacity < this->m_capacity)
		{
			this->reallocate(newCapacity);
		}
		
		return *this;
	}
	DynamicString& DynamicString::replace(size_t start, size_t end, const StaticString& str)
	{
		const size_t nunCodePointsInStr = str.numCodePointsInString();
		const size_t codePointOffsetStart = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, start);
		const size_t codePointOffsetEnd = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(this->m_str, this->m_size, end, start, codePointOffsetStart);
		const size_t newCapacity = (this->m_capacity + nunCodePointsInStr) - (codePointOffsetEnd - codePointOffsetStart);

		const size_t previousCapacity = this->m_capacity;
		if(newCapacity > this->m_capacity)
		{
			this->reallocate(newCapacity);
		}
		fsds::utf8HelperFunctions::selfInsertReplace(this->m_str, previousCapacity, str.data(), nunCodePointsInStr, codePointOffsetStart, codePointOffsetEnd);
		this->m_size += str.size();
		this->m_size -= end - start;
		if(newCapacity < this->m_capacity)
		{
			this->reallocate(newCapacity);
		}
		
		return *this;
	}
	DynamicString& DynamicString::replace(const DynamicString& oldStr, const DynamicString& newStr)
	{
		const size_t start = this->find(oldStr);
		const size_t end = start + oldStr.size();

		this->replace(start, end, newStr);
		
		return *this;
	}
	DynamicString& DynamicString::replace(const DynamicString& oldStr, const StaticString& newStr)
	{
		const size_t start = this->find(oldStr);
		const size_t end = start + oldStr.size();

		this->replace(start, end, newStr);
		
		return *this;
	}
	DynamicString& DynamicString::replace(const StaticString& oldStr, const DynamicString& newStr)
	{
		const size_t start = this->find(oldStr);
		const size_t end = start + oldStr.size();

		this->replace(start, end, newStr);
		
		return *this;
	}
	DynamicString& DynamicString::replace(const StaticString& oldStr, const StaticString& newStr)
	{
		const size_t start = this->find(oldStr);
		const size_t end = start + oldStr.size();

		this->replace(start, end, newStr);
		
		return *this;
	}

	int DynamicString::compare(const DynamicString& other) const
	{
		return fsds::utf8HelperFunctions::compare(this->m_str, this->m_size, other.data(), other.size());
	}
	int DynamicString::compare(const StaticString& other) const
	{
		return fsds::utf8HelperFunctions::compare(this->m_str, this->m_size, other.data(), other.size());
	}

	bool DynamicString::contains(const DynamicString& str) const
	{
		return find(str) != DynamicString::npos;
	}
	bool DynamicString::contains(const StaticString& str) const
	{
		return find(str) != DynamicString::npos;
	}

	size_t DynamicString::find(const DynamicString& str) const
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return DynamicString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size()).index;
		}
	}
	size_t DynamicString::find(const StaticString& str) const
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return DynamicString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size()).index;
		}
	}
	DynamicStringItterator DynamicString::findItterator(const DynamicString& str)
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return DynamicStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos);
			
			if(result.index == DynamicString::npos)
			{
				return DynamicStringItterator(this, true);
			}
			else
			{
				return DynamicStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	DynamicStringItterator DynamicString::findItterator(const StaticString& str)
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return DynamicStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::find(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos);
			
			if(result.index == DynamicString::npos)
			{
				return DynamicStringItterator(this, true);
			}
			else
			{
				return DynamicStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}


	size_t DynamicString::findAnyCharacter(const DynamicString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos).index;
		}
	}
	size_t DynamicString::findAnyCharacter(const StaticString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos).index;
		}
	}
	DynamicStringItterator DynamicString::findAnyCharacterItterator(const DynamicString& str)
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos);
			if(result.index == DynamicString::npos)
			{
				return DynamicStringItterator(this, true);
			}
			else
			{
				return DynamicStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	DynamicStringItterator DynamicString::findAnyCharacterItterator(const StaticString& str)
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos);
			if(result.index == DynamicString::npos)
			{
				return DynamicStringItterator(this, true);
			}
			else
			{
				return DynamicStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	size_t DynamicString::findNotAnyCharacter(const DynamicString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos).index;
		}
	}
	size_t DynamicString::findNotAnyCharacter(const StaticString& str) const
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicString::npos;
		}
		else
		{
			return fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos).index;
		}
	}
	DynamicStringItterator DynamicString::findNotAnyCharacterItterator(const DynamicString& str)
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos);
			if(result.index == DynamicString::npos)
			{
				return DynamicStringItterator(this, true);
			}
			else
			{
				return DynamicStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}
	DynamicStringItterator DynamicString::findNotAnyCharacterItterator(const StaticString& str)
	{
		if(str.size() == 0) [[unlikely]]
		{
			return DynamicStringItterator(this, true);
		}
		else
		{
			auto result = fsds::utf8HelperFunctions::findNotAnyCharacter(this->m_str, this->m_size, str.data(), str.size(), DynamicString::npos);
			if(result.index == DynamicString::npos)
			{
				return DynamicStringItterator(this, true);
			}
			else
			{
				return DynamicStringItterator(this, result.index, result.codePointOffset);
			}
		}
	}

	bool DynamicString::startsWith(const DynamicString& str) const
	{
		return fsds::utf8HelperFunctions::startsWith(this->m_str, this->m_size, str.data(), str.size());
	}
	bool DynamicString::startsWith(const StaticString& str) const
	{
		return fsds::utf8HelperFunctions::startsWith(this->m_str, this->m_size, str.data(), str.size());
	}
	bool DynamicString::endsWith(const DynamicString& str) const
	{
		return fsds::utf8HelperFunctions::endsWith(this->m_str, this->m_size, str.data(), str.size());
	}
	bool DynamicString::endsWith(const StaticString& str) const
	{
		return fsds::utf8HelperFunctions::endsWith(this->m_str, this->m_size, str.data(), str.size());
	}


	bool operator==(const DynamicString& lhs, const DynamicString& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	bool operator!=(const DynamicString& lhs, const DynamicString& rhs)
	{
		return !lhs.valueEquality(rhs);
	}

	bool operator==(const DynamicString& lhs, const StaticString& rhs)
	{
		return lhs.valueEquality(rhs);
	}
	bool operator!=(const DynamicString& lhs, const StaticString& rhs)
	{
		return !lhs.valueEquality(rhs);
	}


	std::ostream& operator<<(std::ostream& os, const DynamicString& str)
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
			char* cstr = new char[str.numCodePointsInString() + 1];
			std::copy(str.data(), str.data() + str.numCodePointsInString(), cstr);
			cstr[str.numCodePointsInString()] = '\0';

			os << cstr;

			delete[] cstr;
		}

		return os;
	}




	DynamicStringItterator::DynamicStringItterator(const DynamicString* sStr)
	: m_str(sStr), m_pos(0), m_chrIndex(0)
	{}
	DynamicStringItterator::DynamicStringItterator(const DynamicString* sStr, const size_t& pos, const size_t& codePoint)
	: m_str(sStr), m_pos(pos), m_chrIndex(codePoint)
	{}
	DynamicStringItterator::DynamicStringItterator(const DynamicString* sStr, const bool& isNPos)
	: m_str(sStr), m_pos(0), m_chrIndex(0)
	{
		if(isNPos)
		{
			this->m_pos = DynamicString::npos;
		}
	}

	DynamicString DynamicStringItterator::get() const
	{
		return DynamicString(m_str->data() + this->m_pos, 1);
	}

	bool DynamicStringItterator::next()
	{
		DynamicString::CharT codePoint = this->m_str->data()[this->m_pos + 1];
		
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
	bool DynamicStringItterator::previous()
	{
		DynamicString::CharT codePoint = this->m_str->data()[this->m_pos - 1];
		
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


	DynamicString DynamicStringItterator::operator*() const
	{
		return this->get();
	}
	DynamicString DynamicStringItterator::operator++()
	{
		this->next();
		return this->get();
	}
	DynamicString DynamicStringItterator::operator++(int)
	{
		DynamicString result = this->get();
		this->next();
		return result;
	}
	DynamicStringItterator& DynamicStringItterator::operator=(const DynamicStringItterator& other)
	{
		this->m_str = other.m_str;
		this->m_pos = other.m_pos;
		this->m_chrIndex = other.m_chrIndex;

		return *this;
	}
	DynamicStringItterator::DynamicStringItterator(const DynamicStringItterator& other)
	: m_str(other.m_str), m_pos(other.m_pos), m_chrIndex(other.m_chrIndex)
	{}

	bool operator==(const DynamicStringItterator& lhs, const DynamicStringItterator& rhs)
	{
		return (lhs.str() == rhs.str()) && (lhs.currentPosition() == rhs.currentPosition()) && (lhs.currentCodePointOffset() == rhs.currentCodePointOffset());
	}
	bool operator!=(const DynamicStringItterator& lhs, const DynamicStringItterator& rhs)
	{
		return !(lhs == rhs);
	}
}