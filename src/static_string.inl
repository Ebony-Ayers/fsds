namespace fsds
{
	constexpr size_t StaticString::size() const
	{
		return this->m_size;
	}
	constexpr size_t StaticString::numCodePointsInString() const
	{
		size_t numCodePoints = 0;
		for(size_t i = 0; i < this->m_size; i++)
		{
			numCodePoints += this->numCodePointsInFirstCharacter(numCodePoints);
		}
		return numCodePoints;
	}
	constexpr bool StaticString::isEmpty() const
	{
		return this->m_size == 0;
	}

	constexpr const StaticString::CharT* StaticString::data() const
	{
		return this->m_str;
	}

	constexpr size_t StaticString::numCodePointsInFirstCharacter(size_t codePointOffset) const
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
					return StaticString::npos;
				}
				return numCodePoints;
			}
		}
	}
	
	constexpr const StaticString* StaticStringItterator::str()
	{
		return this->m_str;
	}
	constexpr size_t StaticStringItterator::currentPosition()
	{
		return this->m_pos;
	}
	constexpr size_t StaticStringItterator::currentCodePointOffset()
	{
		return this->m_chrIndex;
	} 

	constexpr bool StaticStringItterator::isNPos() const
	{
		return (this->m_pos == StaticString::npos);
	}
}