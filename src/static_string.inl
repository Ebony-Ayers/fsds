namespace fsds
{
	constexpr size_t StaticString::size() const
	{
		return this->m_size;
	}
	constexpr bool StaticString::isEmpty() const
	{
		return this->m_size == 0;
	}

	constexpr const StaticString::CharT* StaticString::data() const
	{
		return this->m_str;
	}
	

	constexpr bool StaticStringItterator::isNPos() const
	{
		return (this->m_pos == StaticString::npos);
	}
}