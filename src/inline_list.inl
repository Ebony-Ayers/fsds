namespace fsds
{
	template<typename T>
	constexpr size_t InlineListDataBlock<T>::runtimeSizeOf()
	{
		return sizeof(InlineListDataBlock) + (sizeof(T) * (this->header.capacity - 1));
	}
	template<typename T>
	constexpr size_t InlineListDataBlock<T>::runtimeSizeOfDouble()
	{
		return sizeof(InlineListDataBlock) + (sizeof(T) * ((2 * this->header.capacity) - 1));
	}
	template<typename T>
	template<double factor>
	constexpr size_t InlineListDataBlock<T>::runtimeSizeOfFactorIncrease()
	{
		return sizeof(InlineListDataBlock) + (sizeof(T) * (static_cast<size_t>(factor * static_cast<double>(this->header.capacity)) - 1));
	}
}