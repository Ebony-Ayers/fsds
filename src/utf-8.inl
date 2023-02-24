namespace fsds
{
	namespace utf8HelperFunctions
	{
		template<typename T>
		constexpr size_t numCodePointsInFirstCharacter(const T* const str, const size_t& codePointOffset, const size_t& npos)
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
			
			if((str[codePointOffset] & 0b10000000) == 0b00000000) [[likely]]
			{
				return 1;
			}
			else
			{
				size_t numCodePoints;
				T codePoint = str[codePointOffset];				
				if      ((codePoint & 0b11100000) == 0b11000000)            { numCodePoints = 2; }
				else if ((codePoint & 0b11110000) == 0b11100000)            { numCodePoints = 3; }
				else if ((codePoint & 0b11111000) == 0b11110000)            { numCodePoints = 4; }
				else if ((codePoint & 0b11111100) == 0b11111000)            { numCodePoints = 5; }
				else if ((codePoint & 0b11111110) == 0b11111100)            { numCodePoints = 6; }
				else if ((codePoint & 0b11111111) == 0b11111110)            { numCodePoints = 7; }
				else [[unlikely]]
				{
					return npos;
				}
				return numCodePoints;
			}
		}

		template<typename T>
		constexpr size_t codePointOffsetOfCharacterInString(const T* const str, const size_t& size, const size_t& pos, const size_t& posOffset, const size_t& codePointOffset)
		{
			if(pos > size) [[unlikely]]
			{
				throw std::out_of_range("utf8HelperFunctions::codePointOffsetOfCharacterInString Index out of range");
			}
			else
			{
				size_t internalCodePointOffset = codePointOffset;
				for(size_t i = posOffset; i < pos; i++)
				{
					internalCodePointOffset += fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str, internalCodePointOffset);
				}
				return internalCodePointOffset;
			}
		}
		template<typename T>
		constexpr size_t codePointOffsetOfCharacterInString(const T* const str, const size_t& size, const size_t& pos)
		{
			return codePointOffsetOfCharacterInString(str, size, pos, 0, 0);
		}

		template<typename T>
		constexpr bool equality(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2)
		{
			if((str1 == str2) && (size1 == size2))
			{
				return true;
			}
			else
			{
				size_t codePointOffset = 0;
				for(size_t i = 0; i < size1; i++)
				{
					size_t numCodePoints1 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset);
					size_t numCodePoints2 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffset);

					if(numCodePoints1 != numCodePoints2)
					{
						return false;
					}
					
					for(size_t j = 0; j < numCodePoints1; j++)
					{
						if(str1[codePointOffset + j] != str2[codePointOffset + j])
						{
							return false;
						}
					}

					codePointOffset += numCodePoints1;
				}
				return true;
			}
		}

		template<typename T>
		constexpr int compare(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2)
		{
			size_t codePointOffset1 = 0;
			size_t codePointOffset2 = 0;
			for(size_t i = 0; i < size1; i++)
			{
				if(i >= size2)
				{
					return -1;
				}
				
				size_t numCodePoints1 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset1);
				size_t numCodePoints2 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffset2);
				
				uint64_t unicodeValue1 = 0;
				uint64_t unicodeValue2 = 0;
				
				if(numCodePoints1 == 1)
				{
					unicodeValue1 = static_cast<uint64_t>(str1[codePointOffset1]);
				}
				else
				{
					//copy the data bits (last 6) of the continute character code points
					size_t shiftOffset = 0;
					for(size_t j = 0; j < numCodePoints1 - 1; j++)
					{
						unicodeValue1 += (static_cast<uint64_t>(str1[codePointOffset1 + numCodePoints1 - j - 1]) & 0b00111111) << shiftOffset;
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
					unicodeValue1 += (static_cast<uint64_t>(str1[codePointOffset1]) & mask) << shiftOffset;
				}

				if(numCodePoints2 == 1)
				{
					unicodeValue2 = static_cast<uint64_t>(str2[codePointOffset2]);
				}
				else
				{
					//copy the data bits (last 6) of the continute character code points
					size_t shiftOffset = 0;
					for(size_t j = 0; j < numCodePoints2- 1; j++)
					{
						unicodeValue2 += (static_cast<uint64_t>(str2[codePointOffset2 + numCodePoints2 - j - 1]) & 0b00111111) << shiftOffset;
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
					unicodeValue2 += (static_cast<uint64_t>(str2[codePointOffset2]) & mask) << shiftOffset;
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
			if(size2 > size1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}

		template<typename T>
		constexpr IndexAndOffset find(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2, const size_t& npos)
		{
			size_t codePointOffsetI = 0;
			for(size_t i = 0; i < size1; i++)
			{
				if(size2 + i <= size1)
				{
					bool foundMatch = true;
					
					size_t codePointOffsetJ1 = 0;
					size_t codePointOffsetJ2 = codePointOffsetI;
					for(size_t j = 0; j < size2; j++)
					{
						size_t numCodePoints1 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffsetJ1);
						size_t numCodePoints2 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffsetJ2);

						if(numCodePoints1 == numCodePoints2)
						{
							for(size_t k = 0; k < numCodePoints1; k++)
							{
								if(str1[codePointOffsetJ2 + k] != str2[codePointOffsetJ1 + k])
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
						return {i, codePointOffsetI};
					}
				}
				else
				{
					break;
				}
				
				codePointOffsetI += fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffsetI);
			}
			return {npos, 0};
		}
		template<typename T>
		constexpr IndexAndOffset findFirstCharacter(const T* const str1, const size_t& size1, const T* const character, const size_t& npos)
		{
			size_t numCodePointsInCharacter = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(character);

			size_t codePointOffset = 0;
			for(size_t i = 0; i < size1; i++)
			{
				size_t numCodePoints = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset);
				
				if(numCodePoints == numCodePointsInCharacter)
				{
					bool foundMatch = true;
					for(size_t j = 0; j < numCodePointsInCharacter; j++)
					{
						if(str1[codePointOffset + j] != character[j])
						{
							foundMatch = false;
							break;
						}
					}

					if(foundMatch)
					{
						return {i, codePointOffset};
					}
				}

				codePointOffset += numCodePoints;
			}

			return {npos, 0};
		}
		template<typename T>
		constexpr IndexAndOffset findAnyCharacter(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2, const size_t& npos)
		{
			size_t codePointOffset1 = 0;
			for(size_t i = 0; i < size1; i++)
			{
				size_t numCodePoints1 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset1, npos);
				
				size_t codePointOffset2 = 0;
				for(size_t j = 0; j < size2; j++)
				{
					size_t numCodePoints2 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffset2, npos);
					
					if(numCodePoints1 == numCodePoints2) [[likely]]
					{
						bool foundMatch = true;
						for(size_t k = 0; k < numCodePoints2; k++)
						{
							if(str1[codePointOffset1 + k] != str2[codePointOffset2 + k])
							{
								foundMatch = false;
								break;
							}
						}
						if(foundMatch)
						{
							return {i, codePointOffset1};
						}
					}

					codePointOffset2 += numCodePoints2;
				}

				codePointOffset1 += numCodePoints1;
			}
			return {npos, 0};
		}
		template<typename T>
		constexpr IndexAndOffset findNotAnyCharacter(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2, const size_t& npos)
		{
			size_t codePointOffset1 = 0;
			for(size_t i = 0; i < size1; i++)
			{
				size_t numCodePoints1 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset1, npos);
				
				bool notInStr2 = true;
				size_t codePointOffset2 = 0;
				for(size_t j = 0; j < size2; j++)
				{
					size_t numCodePoints2 = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffset2, npos);
					
					if(numCodePoints1 == numCodePoints2) [[likely]]
					{
						bool foundMatch = true;
						for(size_t k = 0; k < numCodePoints2; k++)
						{
							if(str1[codePointOffset1 + k] != str2[codePointOffset2 + k])
							{
								foundMatch = false;
								break;
							}
						}
						if(foundMatch)
						{
							notInStr2 = false;
							break;
						}
					}

					codePointOffset2 += numCodePoints2;
				}

				if(notInStr2)
				{
					return {i, codePointOffset1};
				}

				codePointOffset1 += numCodePoints1;
			}
			return {npos, 0};
		}

		template<typename T>
		constexpr bool startsWith(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2)
		{
			if((size1 == 0) || (size2 == 0)) [[unlikely]]
			{
				return false;
			}
			else if(size2 > size1) [[unlikely]]
			{
				return false;
			}
			else
			{
				size_t codePointOffset = 0;
				for(size_t i = 0; i < size2; i++)
				{
					size_t numCodePoints = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset);

					if(numCodePoints != fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffset))
					{
						return false;
					}
					
					for(size_t j = 0; j < numCodePoints; j++)
					{
						if(str1[codePointOffset + j] != str2[codePointOffset + j])
						{
							return false;
						}
					}

					codePointOffset += numCodePoints;
				}
				return true;
			}
		}
		template<typename T>
		constexpr bool endsWith(const T* const str1, const size_t& size1, const T* const str2, const size_t& size2)
		{
			if((size1 == 0) || (size2 == 0)) [[unlikely]]
			{
				return false;
			}
			else if(size2 > size1) [[unlikely]]
			{
				return false;
			}
			
			size_t codePointOffset1 = 0;
			for(size_t i = 0; i < (size1 - size2); i++)
			{
				codePointOffset1 += fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset1);
			}

			size_t codePointOffset2 = 0;
			for(size_t i = 0; i < size2; i++)
			{
				size_t numCodePoints = fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str1, codePointOffset1);
				
				if(numCodePoints != fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffset2))
				{
					return false;
				}
				
				for(size_t j = 0; j < numCodePoints; j++)
				{
					if(str1[codePointOffset1 + j] != str2[codePointOffset2 + j])
					{
						return false;
					}
				}

				codePointOffset1 += numCodePoints;
				codePointOffset2 += fsds::utf8HelperFunctions::numCodePointsInFirstCharacter(str2, codePointOffset2);
			}
			return true;
		}

		template<typename T>
		constexpr void concatenate(const T* const str1, const size_t& numCodePoints1, const T* const str2, const size_t& numCodePoints2, T* dest)
		{
			std::copy(str1, str1 + numCodePoints1, dest);
			std::copy(str2, str2 + numCodePoints2, dest + numCodePoints1);
		}

		template<typename T>
		constexpr void insert(const T* const str1, const size_t& numCodePointsFirst1, const size_t& numCodePointsTotal1, const T* const str2, const size_t& numCodePoints2, T* dest)
		{
			std::copy(str1 + numCodePointsFirst1, str1 + numCodePointsTotal1, dest + numCodePointsFirst1 + numCodePoints2);
			std::copy(str2, str2 + numCodePoints2, dest + numCodePointsFirst1);
			std::copy(str1, str1 + numCodePointsFirst1, dest);
		}
		template<typename T>
		constexpr void selfInsertReplace(T* str1, const size_t& size1, const T* const str2, const size_t& numCodePoints2, const size_t& blockTwoStart, const size_t& blockThreeStart)
		{
			const size_t blockTwoCodePointStart = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(str1, size1, blockTwoStart);
			const size_t blockThreeCodePointStart = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(str1, size1, blockThreeStart, blockTwoStart, blockTwoCodePointStart);
			const size_t blockThreeCodePointEnd = fsds::utf8HelperFunctions::codePointOffsetOfCharacterInString(str1, size1, size1, blockThreeStart, blockThreeCodePointStart);

			size_t a1 = blockTwoCodePointStart;
			size_t a2 = blockThreeCodePointEnd;
			size_t a3 = numCodePoints2 + blockThreeCodePointEnd - (blockThreeCodePointStart - blockTwoCodePointStart);
			//std::cout << blockTwoCodePointStart << " " << blockThreeCodePointStart << " " << (blockThreeCodePointStart - blockTwoCodePointStart) << std::endl;
			std::copy_backward(str1 + blockThreeStart, str1 + blockThreeCodePointEnd, str1 + a3);
			//std::copy_backward(str1 + blockThreeCodePointStart, str1 + blockThreeCodePointEnd, str1 + numCodePoints2 + blockThreeCodePointEnd);
			std::copy(str2, str2 + numCodePoints2, str1 + blockTwoCodePointStart);
		}
	}
}