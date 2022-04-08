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

	


	constexpr bool DynamicStringItterator::isNPos() const
	{
		return (this->m_pos == DynamicString::npos);
	}
}