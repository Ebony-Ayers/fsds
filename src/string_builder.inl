namespace fsds
{
	template<typename SizeType>
	constexpr StringBuilder<SizeType>::PartialString StringBuilder<SizeType>::copyInString(const StaticString& str)
	{
		this->m_staticStrings.append(&str);
		PartialString p;
		p.str = str.data();
		p.size = str.size();
		p.numCodePoints = str.numCodePointsInString();
		return p;
	}
	template<typename SizeType>
	constexpr StringBuilder<SizeType>::PartialString StringBuilder<SizeType>::copyInString(const DynamicString& str)
	{
		this->m_dynamicStrings.append(DynamicString(str));
		PartialString p;
		p.str = str.data();
		p.size = str.size();
		p.numCodePoints = str.numCodePointsInString();
		return p;
	}
	template<typename SizeType>
	constexpr SizeType StringBuilder<SizeType>::findPartialStringIndex(const SizeType& pos) const
	{
		if(this->m_partialStringTable.size() == 0) [[likely]]
		{
			SizeType counter;
			for(SizeType i = 0; i < this->m_partialStringTable.size(); i++)
			{
				counter += this->m_partialStringTable[i];
				if(counter > pos)
				{
					return i;
				}
			}
			return this->m_partialStringTable.size() - 1;
		}
		else
		{
			return 0;
		}
	}
}