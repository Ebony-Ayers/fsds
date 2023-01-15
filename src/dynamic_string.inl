namespace fsds
{
	constexpr size_t DynamicString::size() const
	{
		return this->m_size;
	}
	constexpr size_t DynamicString::capacity() const
	{
		return this->m_capacity;
	}
	constexpr size_t DynamicString::numCodePointsInString() const
	{
		return this->m_numCodePoints;
	}
	constexpr bool DynamicString::isEmpty() const
	{
		return this->m_size == 0;
	}

	constexpr DynamicString::CharT* DynamicString::data() const
	{
		return this->m_str;
	}

	constexpr size_t DynamicString::numCodePointsInFirstCharacter(size_t codePointOffset) const
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
					return DynamicString::npos;
				}
				return numCodePoints;
			}
		}
	}

	constexpr bool DynamicString::firstCharacterEquality(const fsds::DynamicString& str) const
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
	constexpr DynamicString::firstCharacterEqualityWithLengthReturnType DynamicString::firstCharacterEqualityWithLength(const fsds::DynamicString& str) const
	{
		size_t numCodePoints = this->numCodePointsInFirstCharacter();
		//if the number of code points are no equal then the two characters cannot be the same and functions as a safety check in the case of one string being a single character of lesser length thus overflowing the smaller string
		if(numCodePoints != str.numCodePointsInFirstCharacter())
		{
			DynamicString::firstCharacterEqualityWithLengthReturnType result = {false, 0};
			return result;
		}
		//it is important not to say two emtry strings start with the first character for future use
		else if((numCodePoints == 0) || (str.isEmpty() == true))
		{
			DynamicString::firstCharacterEqualityWithLengthReturnType result = {false, 0};
			return result;
		}
		else
		{
			for(size_t i = 0; i < numCodePoints; i++)
			{
				if(this->m_str[i] != str.data()[i])
				{
					DynamicString::firstCharacterEqualityWithLengthReturnType result = {false, numCodePoints};
					return result;
				}
			}
			DynamicString::firstCharacterEqualityWithLengthReturnType result = {true, numCodePoints};
			return result;
		}
	}
	
	

	constexpr void DynamicString::reallocate(size_t newSize)
	{
		std::allocator<DynamicString::CharT> alloc;
		DynamicString::CharT* oldData = this->m_str;
		this->m_str = alloc.allocate(newSize);
		std::copy(oldData, oldData + this->m_numCodePoints, this->m_str);
		alloc.deallocate(oldData, this->m_capacity);
		this->m_capacity = newSize;
	}

	


	constexpr bool DynamicStringItterator::isNPos() const
	{
		return (this->m_pos == DynamicString::npos);
	}
}