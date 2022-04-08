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
			size_t codePointOffset = 0;
			for(size_t i = 0; i < pos; i++)
			{
				codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
			}
			return StaticString(this->m_str + codePointOffset, 1);
		}
	}

	const char* StaticString::cstr() const
	{
		return reinterpret_cast<const char*>(this->m_str);
	}

	bool StaticString::valueEquality(const StaticString& other) const
	{
		if(this->referenceEquality(other) == true)
		{
			return true;
		}
		else
		{
			size_t codePointOffset = 0;
			for(size_t i = 0; i < this->m_size; i++)
			{
				size_t numCodePoints1 = this->numCodePointsInFirstCharacter(codePointOffset);
				size_t numCodePoints2 = other.numCodePointsInFirstCharacter(codePointOffset);

				if(numCodePoints1 != numCodePoints2)
				{
					return false;
				}
				
				for(size_t j = 0; j < numCodePoints1; j++)
				{
					if(this->m_str[codePointOffset + j] != other.data()[codePointOffset + j])
					{
						return false;
					}
				}

				codePointOffset += numCodePoints1;
			}
			return true;
		}
	}
	bool StaticString::referenceEquality(const StaticString& other) const
	{
		return ((this->m_str == other.data()) && (this->m_size == other.size()));
	}
	bool StaticString::valueEquality(const DynamicString& other) const
	{
		if(this->referenceEquality(other) == true)
		{
			return true;
		}
		else
		{
			size_t codePointOffset = 0;
			for(size_t i = 0; i < this->m_size; i++)
			{
				size_t numCodePoints1 = this->numCodePointsInFirstCharacter(codePointOffset);
				size_t numCodePoints2 = other.numCodePointsInFirstCharacter(codePointOffset);

				if(numCodePoints1 != numCodePoints2)
				{
					return false;
				}
				
				for(size_t j = 0; j < numCodePoints1; j++)
				{
					if(this->m_str[codePointOffset + j] != other.data()[codePointOffset + j])
					{
						return false;
					}
				}

				codePointOffset += numCodePoints1;
			}
			return true;
		}
	}
	bool StaticString::referenceEquality(const DynamicString& other) const
	{
		return ((this->m_str == other.data()) && (this->m_size == other.size()));
	}

	StaticString StaticString::substr(const size_t& pos, const size_t& len) const
	{
		size_t codePointOffset = 0;
		for(size_t i = 0; i < pos; i++)
		{
			codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
		}
		return StaticString(this->m_str + codePointOffset, len);
	}

	int StaticString::compare(const StaticString& other)
	{
		size_t codePointOffset1 = 0;
		size_t codePointOffset2 = 0;
		for(size_t i = 0; i < this->m_size; i++)
		{
			if(i >= other.size())
			{
				return -1;
			}
			
			size_t numCodePoints1 = this->numCodePointsInFirstCharacter(codePointOffset1);
			size_t numCodePoints2 = other.numCodePointsInFirstCharacter(codePointOffset2);
			
			uint64_t unicodeValue1 = 0;
			uint64_t unicodeValue2 = 0;
			
			if(numCodePoints1 == 1)
			{
				unicodeValue1 = static_cast<uint64_t>(this->m_str[codePointOffset1]);
			}
			else
			{
				//copy the data bits (last 6) of the continute character code points
				size_t shiftOffset = 0;
				for(size_t j = 0; j < numCodePoints1 - 1; j++)
				{
					unicodeValue1 += (static_cast<uint64_t>(this->m_str[codePointOffset1 + numCodePoints1 - j - 1]) & 0b00111111) << shiftOffset;
					shiftOffset += 6;
				}
				
				//copy the data bits of the start character code point
				uint64_t mask;
				if     (numCodePoints1 == 2) { mask = 0b00011111; }
				else if(numCodePoints1 == 3) { mask = 0b00001111; }
				else if(numCodePoints1 == 4) { mask = 0b00000111; }
				else if(numCodePoints1 == 5) { mask = 0b00000011; }
				else if(numCodePoints1 == 6) { mask = 0b00000001; }
				else                         { mask = 0b00000000; }
				unicodeValue1 += (static_cast<uint64_t>(this->m_str[codePointOffset1]) & mask) << shiftOffset;
			}

			if(numCodePoints2 == 1)
			{
				unicodeValue2 = static_cast<uint64_t>(other.data()[codePointOffset2]);
			}
			else
			{
				//copy the data bits (last 6) of the continute character code points
				size_t shiftOffset = 0;
				for(size_t j = 0; j < numCodePoints2- 1; j++)
				{
					unicodeValue2 += (static_cast<uint64_t>(other.data()[codePointOffset2 + numCodePoints2 - j - 1]) & 0b00111111) << shiftOffset;
					shiftOffset += 6;
				}
				
				//copy the data bits of the start character code point
				uint64_t mask;
				if     (numCodePoints2 == 2) { mask = 0b00011111; }
				else if(numCodePoints2 == 3) { mask = 0b00001111; }
				else if(numCodePoints2 == 4) { mask = 0b00000111; }
				else if(numCodePoints2 == 5) { mask = 0b00000011; }
				else if(numCodePoints2 == 6) { mask = 0b00000001; }
				else                         { mask = 0b00000000; }
				unicodeValue2 += (static_cast<uint64_t>(other.data()[codePointOffset2]) & mask) << shiftOffset;
			}

			if(unicodeValue1 != unicodeValue2)
			{
				int64_t comparison = static_cast<int64_t>(unicodeValue1) - static_cast<int64_t>(unicodeValue2);
				if(comparison < 0)
				{
					return 1;
				}else if(comparison == 0)
				{
					return 0;
				}
				else
				{
					return -1;
				}
			}
			
			codePointOffset1 += numCodePoints1;
			codePointOffset2 += numCodePoints2;
		}
		if(other.size() > this->m_size)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	bool StaticString::contains(const StaticString& str)
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
			size_t codePointOffsetI = 0;
			for(size_t i = 0; i < this->m_size; i++)
			{
				if(str.size() + i <= this->m_size)
				{
					bool foundMatch = true;
					
					size_t codePointOffsetJ1 = 0;
					size_t codePointOffsetJ2 = codePointOffsetI;
					for(size_t j = 0; j < str.size(); j++)
					{
						size_t numCodePoints1 = str.numCodePointsInFirstCharacter(codePointOffsetJ1);
						size_t numCodePoints2 = this->numCodePointsInFirstCharacter(codePointOffsetJ2);

						if(numCodePoints1 == numCodePoints2)
						{
							for(size_t k = 0; k < numCodePoints1; k++)
							{
								if(this->m_str[codePointOffsetJ2 + k] != str.data()[codePointOffsetJ1 + k])
								{
									foundMatch = false;
									break;
								}
								if(foundMatch == false)
								{
									break;
								}
							}
						}
						else
						{
							foundMatch = false;
							break;
						}
						
						codePointOffsetJ1 += numCodePoints1;
						codePointOffsetJ2 += numCodePoints2;
					}

					if(foundMatch)
					{
						return i;
					}
				}
				else
				{
					break;
				}
				
				codePointOffsetI += this->numCodePointsInFirstCharacter(codePointOffsetI);
			}
			return StaticString::npos;
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
			size_t codePointOffsetI = 0;
			for(size_t i = 0; i < this->m_size; i++)
			{
				if(str.size() + i <= this->m_size)
				{
					bool foundMatch = true;
					
					size_t codePointOffsetJ1 = 0;
					size_t codePointOffsetJ2 = codePointOffsetI;
					for(size_t j = 0; j < str.size(); j++)
					{
						size_t numCodePoints1 = str.numCodePointsInFirstCharacter(codePointOffsetJ1);
						size_t numCodePoints2 = this->numCodePointsInFirstCharacter(codePointOffsetJ2);

						if(numCodePoints1 == numCodePoints2)
						{
							for(size_t k = 0; k < numCodePoints1; k++)
							{
								if(this->m_str[codePointOffsetJ2 + k] != str.data()[codePointOffsetJ1 + k])
								{
									foundMatch = false;
									break;
								}
								if(foundMatch == false)
								{
									break;
								}
							}
						}
						else
						{
							foundMatch = false;
							break;
						}
						
						codePointOffsetJ1 += numCodePoints1;
						codePointOffsetJ2 += numCodePoints2;
					}

					if(foundMatch)
					{
						return StaticStringItterator(this, i, codePointOffsetI);
					}
				}
				else
				{
					break;
				}
				
				codePointOffsetI += this->numCodePointsInFirstCharacter(codePointOffsetI);
			}
			return StaticStringItterator(this, true);
		}
	}
						

	size_t StaticString::findFirstCharacter(const StaticString& str) const
	{
		size_t codePointOffset = 0;
		for(size_t i = 0; i < this->m_size; i++)
		{
			auto ret = this->firstCharacterEqualityWithLength(str);
			if(ret.result == true)
			{
				return i;
			}
			else
			{
				codePointOffset += ret.numCodePoints;
			}
		}
		return StaticString::npos;
	}
	StaticStringItterator StaticString::findFirstCharacterItterator(const StaticString& str) const
	{
		size_t codePointOffset = 0;
		for(size_t i = 0; i < this->m_size; i++)
		{
			auto ret = this->firstCharacterEqualityWithLength(str);
			if(ret.result == true)
			{
				return StaticStringItterator(this, i, codePointOffset);
			}
			else
			{
				codePointOffset += ret.numCodePoints;
			}
		}
		return StaticStringItterator(this, true);
	}

	bool StaticString::firstCharacterEquality(const StaticString& str) const
	{
		size_t numCodePoints = this->numCodePointsInFirstCharacter();
		//if the number of code points are no equal then the two characters cannot be the same and functions as a safety check in the case of one string being a single character of lesser length thus overflowing the smaller string
		if(numCodePoints != str.numCodePointsInFirstCharacter())
		{
			return false;
		}
		//it is important not to say two emtry strings start with the first character for future use
		else if((numCodePoints == 0) || (str.isEmpty() == true))
		{
			return false;
		}
		else
		{
			for(size_t i = 0; i < numCodePoints; i++)
			{
				if(this->m_str[i] != str.data()[i])
				{
					return false;
				}
			}
			return true;
		}
	}
	StaticString::firstCharacterEqualityWithLengthReturnType StaticString::firstCharacterEqualityWithLength(const StaticString& str) const
	{
		size_t numCodePoints = this->numCodePointsInFirstCharacter();
		//if the number of code points are no equal then the two characters cannot be the same and functions as a safety check in the case of one string being a single character of lesser length thus overflowing the smaller string
		if(numCodePoints != str.numCodePointsInFirstCharacter())
		{
			StaticString::firstCharacterEqualityWithLengthReturnType result = {false, 0};
			return result;
		}
		//it is important not to say two emtry strings start with the first character for future use
		else if((numCodePoints == 0) || (str.isEmpty() == true))
		{
			StaticString::firstCharacterEqualityWithLengthReturnType result = {false, 0};
			return result;
		}
		else
		{
			for(size_t i = 0; i < numCodePoints; i++)
			{
				if(this->m_str[i] != str.data()[i])
				{
					StaticString::firstCharacterEqualityWithLengthReturnType result = {false, numCodePoints};
					return result;
				}
			}
			StaticString::firstCharacterEqualityWithLengthReturnType result = {true, numCodePoints};
			return result;
		}
	}

	bool StaticString::startsWith(const StaticString& str) const
	{
		if(str.size() > this->m_size)
		{
			return false;
		}
		
		size_t codePointOffset = 0;
		for(size_t i = 0; i < str.size(); i++)
		{
			size_t numCodePoints = this->numCodePointsInFirstCharacter(codePointOffset);

			if(numCodePoints != str.numCodePointsInFirstCharacter(codePointOffset))
			{
				return false;
			}
			
			for(size_t j = 0; j < numCodePoints; j++)
			{
				if(this->m_str[codePointOffset + j] != str.data()[codePointOffset + j])
				{
					return false;
				}
			}

			codePointOffset += numCodePoints;
		}
		return true;
	}
	bool StaticString::endsWith(const StaticString& str) const
	{
		if(str.size() > this->m_size)
		{
			return false;
		}
		
		size_t codePointOffset1 = 0;
		for(size_t i = 0; i < (this->m_size - str.size()); i++)
		{
			codePointOffset1 += this->numCodePointsInFirstCharacter(codePointOffset1);
		}

		size_t codePointOffset2 = 0;
		for(size_t i = 0; i < str.size(); i++)
		{
			size_t numCodePoints = this->numCodePointsInFirstCharacter(codePointOffset1);
			
			if(numCodePoints != str.numCodePointsInFirstCharacter(codePointOffset2))
			{
				return false;
			}
			
			for(size_t j = 0; j < numCodePoints; j++)
			{
				if(this->m_str[codePointOffset1 + j] != str.data()[codePointOffset2 + j])
				{
					return false;
				}
			}

			codePointOffset1 += numCodePoints;
			codePointOffset2 += str.numCodePointsInFirstCharacter(codePointOffset2);
		}
		return true;
	}

	size_t StaticString::numCodePointsInFirstCharacter(size_t codePointOffset) const
	{
		//a code point can have on of 3 headers
		//0X - ascii
		//10 - continue character
		//11 - start of of character

		//start of characters have one of 7 options
		//11 0XXXXX - start of 2 byte character
		//11 10XXXX - start of 3 byte character
		//11 110XXX - start of 4 byte character
		//11 1110XX - start of 5 byte character
		//11 11110X - start of 6 byte character
		//11 111110 - start of 7 byte character
		
		if(this->isEmpty() == true) [[unlikely]]
		{
			return 0;
		}
		else
		{
			if((this->m_str[codePointOffset] & 0b10000000) == 0b00000000) [[likely]]
			{
				return 1;
			}
			else
			{
				size_t numCodePoints;
				CharT codePoint = this->m_str[codePointOffset];				
				if      ((codePoint & 0b11100000) == 0b11000000)            { numCodePoints = 2; }
				else if ((codePoint & 0b11110000) == 0b11100000)            { numCodePoints = 3; }
				else if ((codePoint & 0b11111000) == 0b11110000)            { numCodePoints = 4; }
				else if ((codePoint & 0b11111100) == 0b11111000)            { numCodePoints = 5; }
				else if ((codePoint & 0b11111110) == 0b11111100)            { numCodePoints = 6; }
				else if ((codePoint & 0b11111111) == 0b11111110)            { numCodePoints = 7; }
				else [[unlikely]]
				{
					return std::numeric_limits<int>::max();
				}
				return numCodePoints;
			}
		}
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