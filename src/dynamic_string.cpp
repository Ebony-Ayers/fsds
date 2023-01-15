#include "fsds_options.hpp"
#include "dynamic_string.hpp"

#include <iostream>
#include <memory>
#include <algorithm>
#include <limits>

namespace fsds
{
	DynamicString::DynamicString(const CharT* str, const size_t& size)
	: m_str(nullptr), m_size(size), m_capacity(0), m_numCodePoints(0)
	{
		StaticString sString(str, size);

		size_t codePointOffset = 0;
		for(size_t i = 0; i < this->m_size; i++)
		{
			codePointOffset += sString.numCodePointsInFirstCharacter(codePointOffset);
		}
		this->m_capacity = codePointOffset;
		this->m_numCodePoints = codePointOffset;
		
		std::allocator<DynamicString::CharT> alloc;
		this->m_str = alloc.allocate(this->m_capacity);
		
		std::copy(str, str + codePointOffset, this->m_str);
	}
	DynamicString::DynamicString(const CharT* str, const size_t& size, const size_t& numCodePoints)
	: m_str(nullptr), m_size(size), m_capacity(numCodePoints), m_numCodePoints(numCodePoints)
	{
		std::allocator<DynamicString::CharT> alloc;
		this->m_str = alloc.allocate(this->m_capacity);

		std::copy(str, str + numCodePoints, this->m_str);
	}
	DynamicString::DynamicString(const size_t& size, const size_t& numCodePoints)
	: m_str(nullptr), m_size(size), m_capacity(numCodePoints), m_numCodePoints(numCodePoints)
	{
		std::allocator<DynamicString::CharT> alloc;
		this->m_str = alloc.allocate(this->m_capacity);
	}
	DynamicString::DynamicString(const DynamicString& other) noexcept
	: m_str(other.data()), m_size(other.size()), m_capacity(other.capacity()), m_numCodePoints(other.numCodePointsInString())
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
			size_t codePointOffset = 0;
			for(size_t i = 0; i < pos; i++)
			{
				codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
			}
			return DynamicString(this->m_str + codePointOffset, 1);
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
					this->m_numCodePoints += numCodePoints2 - numCodePoints1;
				}
				else
				{
					this->m_numCodePoints -= numCodePoints1 - numCodePoints2;
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
		if((this->m_str == other.data()) && (this->m_size == other.size()))
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
	bool DynamicString::valueEquality(const StaticString& other) const
	{
		if((this->m_str == other.data()) && (this->m_size == other.size()))
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

	DynamicString DynamicString::substr(const size_t& pos, const size_t& len) const
	{
		size_t codePointOffset = 0;
		for(size_t i = 0; i < pos; i++)
		{
			codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
		}
		return DynamicString(this->m_str + codePointOffset, len);
	}

	DynamicString& DynamicString::concatenate(const DynamicString& str)
	{
		if(this->m_numCodePoints + str.numCodePointsInString() > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + str.numCodePointsInString()) * 2);
		}

		std::copy(str.data(), str.data() + str.numCodePointsInString(), this->m_str + this->m_numCodePoints);
		this->m_size += str.size();
		this->m_numCodePoints += str.numCodePointsInString();
		
		return *this;
	}
	DynamicString& DynamicString::concatenate(const StaticString& str)
	{
		auto nunCodePointsInStr = str.numCodePointsInString();
		if(this->m_numCodePoints + nunCodePointsInStr > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + nunCodePointsInStr) * 2);
		}

		std::copy(str.data(), str.data() + nunCodePointsInStr, this->m_str + this->m_numCodePoints);
		this->m_size += str.size();
		this->m_numCodePoints += nunCodePointsInStr;
		
		return *this;
	}
	DynamicString& DynamicString::concatenateFront(const DynamicString& str)
	{
		if(this->m_numCodePoints + str.numCodePointsInString() > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + str.numCodePointsInString()) * 2);
		}

		std::copy(this->m_str, this->m_str + this->m_numCodePoints, this->m_str + str.m_numCodePoints);
		std::copy(str.data(), str.data() + str.numCodePointsInString(), this->m_str);
		this->m_size += str.size();
		this->m_numCodePoints += str.numCodePointsInString();
		
		return *this;
	}
	DynamicString& DynamicString::concatenateFront(const StaticString& str)
	{
		auto nunCodePointsInStr = str.numCodePointsInString();
		if(this->m_numCodePoints + nunCodePointsInStr > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + nunCodePointsInStr) * 2);
		}

		std::copy(this->m_str, this->m_str + this->m_numCodePoints, this->m_str + nunCodePointsInStr);
		std::copy(str.data(), str.data() + nunCodePointsInStr, this->m_str);
		this->m_size += str.size();
		this->m_numCodePoints += nunCodePointsInStr;
		
		return *this;
	}

	DynamicString& DynamicString::insert(size_t pos, const DynamicString& str)
	{
		size_t codePointOffset = 0;
		size_t i;
		for(i = 0; i < pos; i++)
		{
			codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
		}

		if(this->m_numCodePoints + str.numCodePointsInString() > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + str.numCodePointsInString()) * 2);
		}

		std::copy(this->m_str + codePointOffset, this->m_str + this->m_numCodePoints, this->m_str + codePointOffset + str.numCodePointsInString());
		std::copy(str.data(), str.data() + str.numCodePointsInString(), this->m_str + codePointOffset);
		
		this->m_size += str.size();
		this->m_numCodePoints += str.numCodePointsInString();

		return *this;
	}
	DynamicString& DynamicString::insert(size_t pos, const StaticString& str)
	{
		auto nunCodePointsInStr = str.numCodePointsInString();
		size_t codePointOffset = 0;
		size_t i;
		for(i = 0; i < pos; i++)
		{
			codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
		}

		if(this->m_numCodePoints + nunCodePointsInStr > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + nunCodePointsInStr) * 2);
		}

		std::copy(this->m_str + codePointOffset, this->m_str + this->m_numCodePoints, this->m_str + codePointOffset + str.numCodePointsInString());
		std::copy(str.data(), str.data() + nunCodePointsInStr, this->m_str + codePointOffset);
		
		this->m_size += str.size();
		this->m_numCodePoints += nunCodePointsInStr;

		return *this;
	}

	DynamicString& DynamicString::replace(size_t start, size_t end, const DynamicString& str)
	{
		size_t codePointOffsetStart = 0;
		size_t i;
		for(i = 0; i < start; i++)
		{
			codePointOffsetStart += this->numCodePointsInFirstCharacter(codePointOffsetStart);
		}
		size_t codePointOffsetEnd = codePointOffsetStart;
		for(; i < end; i++)
		{
			codePointOffsetEnd += this->numCodePointsInFirstCharacter(codePointOffsetEnd);
		}

		if(this->m_numCodePoints + str.numCodePointsInString() > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + str.numCodePointsInString()) * 2);
		}

		std::copy(this->m_str + codePointOffsetEnd, this->m_str + this->m_numCodePoints, this->m_str + codePointOffsetStart + str.numCodePointsInString());
		std::copy(str.data(), str.data() + str.numCodePointsInString(), this->m_str + codePointOffsetStart);
		
		this->m_size -= end - start;
		this->m_size += str.size();
		this->m_numCodePoints -= codePointOffsetEnd - codePointOffsetStart;
		this->m_numCodePoints += str.numCodePointsInString();
		
		return *this;
	}
	DynamicString& DynamicString::replace(size_t start, size_t end, const StaticString& str)
	{
		auto nunCodePointsInStr = str.numCodePointsInString();
		size_t codePointOffsetStart = 0;
		size_t i;
		for(i = 0; i < start; i++)
		{
			codePointOffsetStart += this->numCodePointsInFirstCharacter(codePointOffsetStart);
		}
		size_t codePointOffsetEnd = codePointOffsetStart;
		for(; i < end; i++)
		{
			codePointOffsetEnd += this->numCodePointsInFirstCharacter(codePointOffsetEnd);
		}

		if(this->m_numCodePoints + nunCodePointsInStr > this->m_capacity) [[unlikely]]
		{
			this->reallocate((this->m_capacity + nunCodePointsInStr) * 2);
		}

		std::copy(this->m_str + codePointOffsetEnd, this->m_str + this->m_numCodePoints, this->m_str + codePointOffsetStart + str.numCodePointsInString());
		std::copy(str.data(), str.data() + nunCodePointsInStr, this->m_str + codePointOffsetStart);
		
		this->m_size -= end - start;
		this->m_size += str.size();
		this->m_numCodePoints -= codePointOffsetEnd - codePointOffsetStart;
		this->m_numCodePoints += nunCodePointsInStr;
		
		return *this;
	}
	DynamicString& DynamicString::replace(const DynamicString& oldStr, const DynamicString& newStr)
	{
		for(size_t i = this->m_size - oldStr.size(); i < this->m_size; i--)
		{
			size_t codePointOffset = 0;
			for(size_t j = 0; j < i; j++)
			{
				codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
			}

			StaticString sStr(this->m_str + codePointOffset, oldStr.size());

			if(oldStr == sStr)
			{
				if((this->m_numCodePoints + newStr.numCodePointsInString()) - oldStr.numCodePointsInString() >= this->m_capacity) [[unlikely]]
				{
					this->reallocate(((this->m_capacity + newStr.numCodePointsInString()) - oldStr.numCodePointsInString()) * 2);
				}
				
				std::copy(this->m_str + codePointOffset + oldStr.numCodePointsInString(), this->m_str + this->m_numCodePoints, this->m_str + codePointOffset + newStr.numCodePointsInString());
				std::copy(newStr.data(), newStr.data() + newStr.numCodePointsInString(), this->m_str + codePointOffset);
				this->m_numCodePoints -= oldStr.numCodePointsInString();
				this->m_numCodePoints += newStr.numCodePointsInString();
				this->m_size -= oldStr.size();
				this->m_size += newStr.size();
				std::cout << "replace " << i << std::endl;
			}
		}
		
		return *this;
	}
	DynamicString& DynamicString::replace(const DynamicString& oldStr, const StaticString& newStr)
	{
		auto nunCodePointsInNewStr = newStr.numCodePointsInString();
		for(size_t i = this->m_size - oldStr.size(); i < this->m_size; i--)
		{
			size_t codePointOffset = 0;
			for(size_t j = 0; j < i; j++)
			{
				codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
			}

			StaticString sStr(this->m_str + codePointOffset, oldStr.size());

			if(oldStr == sStr)
			{
				if((this->m_numCodePoints + nunCodePointsInNewStr) - oldStr.numCodePointsInString() >= this->m_capacity) [[unlikely]]
				{
					this->reallocate(((this->m_capacity + nunCodePointsInNewStr) - oldStr.numCodePointsInString()) * 2);
				}
				
				std::copy(this->m_str + codePointOffset + oldStr.numCodePointsInString(), this->m_str + this->m_numCodePoints, this->m_str + codePointOffset + newStr.numCodePointsInString());
				std::copy(newStr.data(), newStr.data() + nunCodePointsInNewStr, this->m_str + codePointOffset);
				this->m_numCodePoints -= oldStr.numCodePointsInString();
				this->m_numCodePoints += nunCodePointsInNewStr;
				this->m_size -= oldStr.size();
				this->m_size += newStr.size();
				std::cout << "replace " << i << std::endl;
			}
		}
		
		return *this;
	}
	DynamicString& DynamicString::replace(const StaticString& oldStr, const DynamicString& newStr)
	{
		auto nunCodePointsInOldStr = oldStr.numCodePointsInString();
		for(size_t i = this->m_size - oldStr.size(); i < this->m_size; i--)
		{
			size_t codePointOffset = 0;
			for(size_t j = 0; j < i; j++)
			{
				codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
			}

			StaticString sStr(this->m_str + codePointOffset, oldStr.size());

			if(oldStr == sStr)
			{
				if((this->m_numCodePoints + newStr.numCodePointsInString()) - nunCodePointsInOldStr >= this->m_capacity) [[unlikely]]
				{
					this->reallocate(((this->m_capacity + newStr.numCodePointsInString()) - nunCodePointsInOldStr) * 2);
				}
				
				std::copy(this->m_str + codePointOffset + nunCodePointsInOldStr, this->m_str + this->m_numCodePoints, this->m_str + codePointOffset + newStr.numCodePointsInString());
				std::copy(newStr.data(), newStr.data() + newStr.numCodePointsInString(), this->m_str + codePointOffset);
				this->m_numCodePoints -= nunCodePointsInOldStr;
				this->m_numCodePoints += newStr.numCodePointsInString();
				this->m_size -= oldStr.size();
				this->m_size += newStr.size();
				std::cout << "replace " << i << std::endl;
			}
		}
		
		return *this;
	}
	DynamicString& DynamicString::replace(const StaticString& oldStr, const StaticString& newStr)
	{
		auto nunCodePointsInOldStr = oldStr.numCodePointsInString();
		auto nunCodePointsInNewStr = newStr.numCodePointsInString();
		for(size_t i = this->m_size - oldStr.size(); i < this->m_size; i--)
		{
			size_t codePointOffset = 0;
			for(size_t j = 0; j < i; j++)
			{
				codePointOffset += this->numCodePointsInFirstCharacter(codePointOffset);
			}

			StaticString sStr(this->m_str + codePointOffset, oldStr.size());

			if(oldStr == sStr)
			{
				if((this->m_numCodePoints + nunCodePointsInNewStr) - nunCodePointsInOldStr >= this->m_capacity) [[unlikely]]
				{
					this->reallocate(((this->m_capacity + nunCodePointsInNewStr) - nunCodePointsInOldStr) * 2);
				}
				
				std::copy(this->m_str + codePointOffset + nunCodePointsInOldStr, this->m_str + this->m_numCodePoints, this->m_str + codePointOffset + newStr.numCodePointsInString());
				std::copy(newStr.data(), newStr.data() + nunCodePointsInNewStr, this->m_str + codePointOffset);
				this->m_numCodePoints -= nunCodePointsInOldStr;
				this->m_numCodePoints += nunCodePointsInNewStr;
				this->m_size -= oldStr.size();
				this->m_size += newStr.size();
				std::cout << "replace " << i << std::endl;
			}
		}
		
		return *this;
	}

	int DynamicString::compare(const DynamicString& other) const
	{
		return this->compare(static_cast<StaticString>(other));
	}
	int DynamicString::compare(const StaticString& other) const
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
		return this->find(static_cast<StaticString>(str));
	}
	size_t DynamicString::find(const StaticString& str) const
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return DynamicString::npos;
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
			return DynamicString::npos;
		}
	}
	DynamicStringItterator DynamicString::findItterator(const DynamicString& str)
	{
		return this->findItterator(static_cast<StaticString>(str));
	}
	DynamicStringItterator DynamicString::findItterator(const StaticString& str)
	{
		if(str.size() > this->m_size) [[unlikely]]
		{
			return DynamicStringItterator(this, true);
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
						return DynamicStringItterator(this, i, codePointOffsetI);
					}
				}
				else
				{
					break;
				}
				
				codePointOffsetI += this->numCodePointsInFirstCharacter(codePointOffsetI);
			}
			return DynamicStringItterator(this, true);
		}
	}
						

	size_t DynamicString::findFirstCharacter(const DynamicString& str) const
	{
		return this->findFirstCharacter(static_cast<StaticString>(str));
	}
	size_t DynamicString::findFirstCharacter(const StaticString& str) const
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
		return DynamicString::npos;
	}
	DynamicStringItterator DynamicString::findFirstCharacterItterator(const DynamicString& str)
	{
		return this->findFirstCharacterItterator(static_cast<StaticString>(str));
	}
	DynamicStringItterator DynamicString::findFirstCharacterItterator(const StaticString& str)
	{
		size_t codePointOffset = 0;
		for(size_t i = 0; i < this->m_size; i++)
		{
			auto ret = this->firstCharacterEqualityWithLength(str);
			if(ret.result == true)
			{
				return DynamicStringItterator(this, i, codePointOffset);
			}
			else
			{
				codePointOffset += ret.numCodePoints;
			}
		}
		return DynamicStringItterator(this, true);
	}

	bool DynamicString::startsWith(const DynamicString& str) const
	{
		if(str.numCodePointsInString() > this->m_numCodePoints)
		{
			return false;
		}
		if(str.size() > this->m_size)
		{
			return false;
		}

		for(size_t i = 0; i < str.numCodePointsInString(); i++)
		{
			if(this->m_str[i] != str.data()[i])
			{
				return false;
			}
		}
		return true;
	}
	bool DynamicString::startsWith(const StaticString& str) const
	{
		auto nunCodePointsInStr = str.numCodePointsInString();
		if(nunCodePointsInStr > this->m_numCodePoints)
		{
			return false;
		}
		if(str.size() > this->m_size)
		{
			return false;
		}

		for(size_t i = 0; i < nunCodePointsInStr; i++)
		{
			if(this->m_str[i] != str.data()[i])
			{
				return false;
			}
		}
		return true;
	}
	bool DynamicString::endsWith(const DynamicString& str) const
	{
		if(str.numCodePointsInString() > this->m_numCodePoints)
		{
			return false;
		}
		if(str.size() > this->m_size)
		{
			return false;
		}

		for(size_t i = 0; i < str.numCodePointsInString(); i++)
		{
			if(this->m_str[this->m_numCodePoints - i] != str.data()[str.numCodePointsInString() - i])
			{
				return false;
			}
		}
		return true;
	}
	bool DynamicString::endsWith(const StaticString& str) const
	{
		auto numCodePointsInStr = str.numCodePointsInString();
		if(numCodePointsInStr > this->m_numCodePoints)
		{
			return false;
		}
		if(str.size() > this->m_size)
		{
			return false;
		}

		for(size_t i = 0; i < numCodePointsInStr; i++)
		{
			if(this->m_str[this->m_numCodePoints - i] != str.data()[numCodePointsInStr - i])
			{
				return false;
			}
		}
		return true;
	}

	bool DynamicString::firstCharacterEquality(const StaticString& str) const
	{
		auto numCodePointsInStr = this->numCodePointsInFirstCharacter();
		//if the number of code points are no equal then the two characters cannot be the same and functions as a safety check in the case of one string being a single character of lesser length thus overflowing the smaller string
		if(numCodePointsInStr != str.numCodePointsInFirstCharacter())
		{
			return false;
		}
		//it is important not to say two emtry strings start with the first character for future use
		else if((numCodePointsInStr == 0) || (str.isEmpty() == true))
		{
			return false;
		}
		else
		{
			for(size_t i = 0; i < numCodePointsInStr; i++)
			{
				if(this->m_str[i] != str.data()[i])
				{
					return false;
				}
			}
			return true;
		}
	}
	DynamicString::firstCharacterEqualityWithLengthReturnType DynamicString::firstCharacterEqualityWithLength(const StaticString& str) const
	{
		auto numCodePointsInStr = this->numCodePointsInFirstCharacter();
		//if the number of code points are no equal then the two characters cannot be the same and functions as a safety check in the case of one string being a single character of lesser length thus overflowing the smaller string
		if(numCodePointsInStr != str.numCodePointsInFirstCharacter())
		{
			DynamicString::firstCharacterEqualityWithLengthReturnType result = {false, 0};
			return result;
		}
		//it is important not to say two emtry strings start with the first character for future use
		else if((numCodePointsInStr == 0) || (str.isEmpty() == true))
		{
			DynamicString::firstCharacterEqualityWithLengthReturnType result = {false, 0};
			return result;
		}
		else
		{
			for(size_t i = 0; i < numCodePointsInStr; i++)
			{
				if(this->m_str[i] != str.data()[i])
				{
					DynamicString::firstCharacterEqualityWithLengthReturnType result = {false, numCodePointsInStr};
					return result;
				}
			}
			DynamicString::firstCharacterEqualityWithLengthReturnType result = {true, numCodePointsInStr};
			return result;
		}
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



	DynamicStringItterator::DynamicStringItterator(DynamicString* sStr)
	: m_str(sStr), m_pos(0), m_chrIndex(0)
	{}
	DynamicStringItterator::DynamicStringItterator(DynamicString* sStr, const size_t& pos, const size_t& codePoint)
	: m_str(sStr), m_pos(pos), m_chrIndex(codePoint)
	{}
	DynamicStringItterator::DynamicStringItterator(DynamicString* sStr, const bool& isNPos)
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
	void DynamicStringItterator::set(const DynamicString& character)
	{
		this->m_str->set(this->m_chrIndex, character);
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
}