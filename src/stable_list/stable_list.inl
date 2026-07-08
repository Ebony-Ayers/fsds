namespace fsds
{
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::Iterator::Iterator() noexcept
        : m_iRef(), m_list(nullptr), m_step(1) {}
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::Iterator::Iterator(IRef iRef, StableList* list, int step) noexcept
        : m_iRef(iRef), m_list(list), m_step(step) {}

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::Iterator::reference
    StableList<T, Allocator>::Iterator::operator*() const
    {
        return m_list->at(m_iRef);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::Iterator::pointer
    StableList<T, Allocator>::Iterator::operator->() const
    {
        return &m_list->at(m_iRef);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::Iterator::reference
    StableList<T, Allocator>::Iterator::operator[](difference_type n) const
    {
        return m_list->at(IRef{m_iRef.value + static_cast<size_t>(static_cast<difference_type>(m_step) * n)});
    }

    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::Iterator&
    StableList<T, Allocator>::Iterator::operator++() noexcept
    {
        m_iRef.value += static_cast<size_t>(m_step);
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::Iterator
    StableList<T, Allocator>::Iterator::operator++(int) noexcept
    {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::Iterator&
    StableList<T, Allocator>::Iterator::operator--() noexcept
    {
        m_iRef.value -= static_cast<size_t>(m_step);
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::Iterator
    StableList<T, Allocator>::Iterator::operator--(int) noexcept
    {
        Iterator tmp = *this;
        --(*this);
        return tmp;
    }

    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::Iterator&
    StableList<T, Allocator>::Iterator::operator+=(difference_type n) noexcept
    {
        m_iRef.value += static_cast<size_t>(static_cast<difference_type>(m_step) * n);
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::Iterator&
    StableList<T, Allocator>::Iterator::operator-=(difference_type n) noexcept
    {
        m_iRef.value -= static_cast<size_t>(static_cast<difference_type>(m_step) * n);
        return *this;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::Iterator
    StableList<T, Allocator>::Iterator::operator+(difference_type n) const noexcept
    {
        Iterator tmp = *this;
        tmp += n;
        return tmp;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::Iterator
    StableList<T, Allocator>::Iterator::operator-(difference_type n) const noexcept
    {
        Iterator tmp = *this;
        tmp -= n;
        return tmp;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::Iterator::difference_type
    StableList<T, Allocator>::Iterator::operator-(const Iterator& other) const noexcept
    {
        return static_cast<difference_type>(m_iRef.value - other.m_iRef.value) / m_step;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::Iterator::operator==(const Iterator& other) const noexcept
    {
        return m_iRef == other.m_iRef;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::Iterator::operator!=(const Iterator& other) const noexcept
    {
        return m_iRef != other.m_iRef;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::Iterator::operator<(const Iterator& other) const noexcept
    {
        return (m_step > 0) ? (m_iRef.value < other.m_iRef.value) : (m_iRef.value > other.m_iRef.value);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::Iterator::operator>(const Iterator& other) const noexcept
    {
        return other < *this;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::Iterator::operator<=(const Iterator& other) const noexcept
    {
        return !(other < *this);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::Iterator::operator>=(const Iterator& other) const noexcept
    {
        return !(*this < other);
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::IRef
    StableList<T, Allocator>::Iterator::iRef() const noexcept
    {
        return m_iRef;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr int StableList<T, Allocator>::Iterator::step() const noexcept
    {
        return m_step;
    }

    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::ConstIterator::ConstIterator() noexcept
        : m_iRef(), m_list(nullptr), m_step(1) {}
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::ConstIterator::ConstIterator(IRef iRef, const StableList* list, int step) noexcept
        : m_iRef(iRef), m_list(list), m_step(step) {}
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::ConstIterator::ConstIterator(const Iterator& it) noexcept
        : m_iRef(it.m_iRef), m_list(it.m_list), m_step(it.m_step) {}

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::ConstIterator::reference
    StableList<T, Allocator>::ConstIterator::operator*() const
    {
        return m_list->at(m_iRef);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::ConstIterator::pointer
    StableList<T, Allocator>::ConstIterator::operator->() const
    {
        return &m_list->at(m_iRef);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::ConstIterator::reference
    StableList<T, Allocator>::ConstIterator::operator[](difference_type n) const
    {
        return m_list->at(IRef{m_iRef.value + static_cast<size_t>(static_cast<difference_type>(m_step) * n)});
    }

    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::ConstIterator&
    StableList<T, Allocator>::ConstIterator::operator++() noexcept
    {
        m_iRef.value += static_cast<size_t>(m_step);
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::ConstIterator
    StableList<T, Allocator>::ConstIterator::operator++(int) noexcept
    {
        ConstIterator tmp = *this;
        ++(*this);
        return tmp;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::ConstIterator&
    StableList<T, Allocator>::ConstIterator::operator--() noexcept
    {
        m_iRef.value -= static_cast<size_t>(m_step);
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::ConstIterator
    StableList<T, Allocator>::ConstIterator::operator--(int) noexcept
    {
        ConstIterator tmp = *this;
        --(*this);
        return tmp;
    }

    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::ConstIterator&
    StableList<T, Allocator>::ConstIterator::operator+=(difference_type n) noexcept
    {
        m_iRef.value += static_cast<size_t>(static_cast<difference_type>(m_step) * n);
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr typename StableList<T, Allocator>::ConstIterator&
    StableList<T, Allocator>::ConstIterator::operator-=(difference_type n) noexcept
    {
        m_iRef.value -= static_cast<size_t>(static_cast<difference_type>(m_step) * n);
        return *this;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::ConstIterator
    StableList<T, Allocator>::ConstIterator::operator+(difference_type n) const noexcept
    {
        ConstIterator tmp = *this;
        tmp += n;
        return tmp;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::ConstIterator
    StableList<T, Allocator>::ConstIterator::operator-(difference_type n) const noexcept
    {
        ConstIterator tmp = *this;
        tmp -= n;
        return tmp;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::ConstIterator::difference_type
    StableList<T, Allocator>::ConstIterator::operator-(const ConstIterator& other) const noexcept
    {
        return static_cast<difference_type>(m_iRef.value - other.m_iRef.value) / m_step;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::ConstIterator::operator==(const ConstIterator& other) const noexcept
    {
        return m_iRef == other.m_iRef;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::ConstIterator::operator!=(const ConstIterator& other) const noexcept
    {
        return m_iRef != other.m_iRef;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::ConstIterator::operator<(const ConstIterator& other) const noexcept
    {
        return (m_step > 0) ? (m_iRef.value < other.m_iRef.value) : (m_iRef.value > other.m_iRef.value);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::ConstIterator::operator>(const ConstIterator& other) const noexcept
    {
        return other < *this;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::ConstIterator::operator<=(const ConstIterator& other) const noexcept
    {
        return !(other < *this);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::ConstIterator::operator>=(const ConstIterator& other) const noexcept
    {
        return !(*this < other);
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::IRef
    StableList<T, Allocator>::ConstIterator::iRef() const noexcept
    {
        return m_iRef;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr int StableList<T, Allocator>::ConstIterator::step() const noexcept
    {
        return m_step;
    }

    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList()
    {
        int retcode = FSDS_StableList_constructDefault(&this->m_list, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(const Allocator& /*alloc*/)
    {
        int retcode = FSDS_StableList_constructDefault(&this->m_list, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(size_t count, const Allocator& /*alloc*/)
    {
        int retcode = FSDS_StableList_constructSize(&this->m_list, count, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        if constexpr(!std::is_trivially_constructible<T>::value)
        {
            for(size_t i = 0; i < count; i++)
            {
                T* ptr;
                retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
                if(retcode)
                {
                    throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
                }
                new(ptr) T();
            }
        }
        else
        {
            this->m_list.memBlock->header.size = count;
        }
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(size_t count, const T& value, const Allocator& /*alloc*/)
    {
        int retcode = FSDS_StableList_constructSize(&this->m_list, count, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(size_t i = 0; i < count; i++)
        {
            T* ptr;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(ptr) T(value);
        }
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(const StableList& other)
    {
        size_t otherSize = FSDS_StableList_size(other.m_list);
        int retcode = FSDS_StableList_constructSize(&this->m_list, otherSize, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(size_t i = 0; i < otherSize; i++)
        {
            T* src;
            retcode = FSDS_StableList_getElementIndex(other.m_list, i, reinterpret_cast<void**>(&src));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            T* dst;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&dst));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(dst) T(*src);
        }
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(const StableList& other, const Allocator& /*alloc*/)
    : StableList(other)
    {
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(StableList&& other)
    {
        this->m_list.memBlock = other.m_list.memBlock;
        other.m_list.memBlock = nullptr;
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(StableList&& other, const Allocator& /*alloc*/)
    {
        this->m_list.memBlock = other.m_list.memBlock;
        other.m_list.memBlock = nullptr;
    }
    template<typename T, typename Allocator>
    template<typename InputIt>
    constexpr StableList<T, Allocator>::StableList(InputIt first, InputIt last, const Allocator& /*alloc*/)
    {
        int retcode = FSDS_StableList_constructDefault(&this->m_list, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(InputIt it = first; it != last; ++it)
        {
            T* ptr;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(ptr) T(*it);
        }
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::StableList(std::initializer_list<T> init, const Allocator& /*alloc*/)
    {
        int retcode = FSDS_StableList_constructSize(&this->m_list, init.size(), sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(const T& val : init)
        {
            T* ptr;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(ptr) T(val);
        }
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>::~StableList()
    {
        if(this->m_list.memBlock == nullptr) return;
        if constexpr(!std::is_trivially_destructible<T>::value)
        {
            size_t count = FSDS_StableList_size(this->m_list);
            for(size_t i = 0; i < count; i++)
            {
                T* ptr;
                int retcode = FSDS_StableList_getElementIndex(this->m_list, i, reinterpret_cast<void**>(&ptr));
                if(retcode)
                {
                    throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
                }
                std::destroy_at(ptr);
            }
        }
        FSDS_StableList_destroy(this->m_list);
    }

    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>& StableList<T, Allocator>::operator=(const StableList& other)
    {
        if(this == &other) return *this;
        this->~StableList();
        size_t otherSize = FSDS_StableList_size(other.m_list);
        int retcode = FSDS_StableList_constructSize(&this->m_list, otherSize, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(size_t i = 0; i < otherSize; i++)
        {
            T* src;
            retcode = FSDS_StableList_getElementIndex(other.m_list, i, reinterpret_cast<void**>(&src));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            T* dst;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&dst));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(dst) T(*src);
        }
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>& StableList<T, Allocator>::operator=(StableList&& other) noexcept
    {
        if(this == &other) return *this;
        this->~StableList();
        this->m_list.memBlock = other.m_list.memBlock;
        other.m_list.memBlock = nullptr;
        return *this;
    }
    template<typename T, typename Allocator>
    constexpr StableList<T, Allocator>& StableList<T, Allocator>::operator=(std::initializer_list<T> ilist)
    {
        this->~StableList();
        int retcode = FSDS_StableList_constructSize(&this->m_list, ilist.size(), sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(const T& val : ilist)
        {
            T* ptr;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(ptr) T(val);
        }
        return *this;
    }

    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::assign(size_t count, const T& value)
    {
        this->~StableList();
        int retcode = FSDS_StableList_constructSize(&this->m_list, count, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(size_t i = 0; i < count; i++)
        {
            T* ptr;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(ptr) T(value);
        }
    }
    template<typename T, typename Allocator>
    template<typename InputIt>
    constexpr void StableList<T, Allocator>::assign(InputIt first, InputIt last)
    {
        this->~StableList();
        int retcode = FSDS_StableList_constructDefault(&this->m_list, sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(InputIt it = first; it != last; ++it)
        {
            T* ptr;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(ptr) T(*it);
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::assign(std::initializer_list<T> ilist)
    {
        this->~StableList();
        int retcode = FSDS_StableList_constructSize(&this->m_list, ilist.size(), sizeof(T));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        for(const T& val : ilist)
        {
            T* ptr;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(ptr) T(val);
        }
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr Allocator StableList<T, Allocator>::get_allocator() const
    {
        return Allocator();
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr T& StableList<T, Allocator>::at(size_t index)
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, index, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr const T& StableList<T, Allocator>::at(size_t index) const
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, index, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr T& StableList<T, Allocator>::at(const IRef& iRef)
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIRef(this->m_list, iRef.value, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr const T& StableList<T, Allocator>::at(const IRef& iRef) const
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIRef(this->m_list, iRef.value, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr T& StableList<T, Allocator>::operator[](size_t index)
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, index, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr const T& StableList<T, Allocator>::operator[](size_t index) const
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, index, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr T& StableList<T, Allocator>::operator[](const IRef& iRef)
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIRef(this->m_list, iRef.value, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr const T& StableList<T, Allocator>::operator[](const IRef& iRef) const
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIRef(this->m_list, iRef.value, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr T& StableList<T, Allocator>::front()
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, 0, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr const T& StableList<T, Allocator>::front() const
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, 0, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr StableList<T, Allocator>::IRef StableList<T, Allocator>::frontIRef() const
    {
        FSDS_StableList_IRef raw;
        int retcode = FSDS_StableList_indexToIRef(this->m_list, 0, &raw);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return IRef{raw};
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr T& StableList<T, Allocator>::back()
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, FSDS_StableList_size(this->m_list) - 1, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr const T& StableList<T, Allocator>::back() const
    {
        T* elemenmt;
        int retcode = FSDS_StableList_getElementIndex(this->m_list, FSDS_StableList_size(this->m_list) - 1, reinterpret_cast<void**>(&elemenmt));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return *elemenmt;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr StableList<T, Allocator>::IRef StableList<T, Allocator>::backIRef() const
    {
        FSDS_StableList_IRef raw;
        int retcode = FSDS_StableList_indexToIRef(this->m_list, FSDS_StableList_size(this->m_list) - 1, &raw);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return IRef{raw};
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr T* StableList<T, Allocator>::data()
    {
        return reinterpret_cast<T*>(this->m_list.memBlock->data + (this->m_list.memBlock->header.front * sizeof(T)));
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr const T* StableList<T, Allocator>::data() const
    {
        return reinterpret_cast<const T*>(this->m_list.memBlock->data + (this->m_list.memBlock->header.front * sizeof(T)));
    }

    // baseIRef is computed directly from the header rather than via FSDS_StableList_indexToIRef
    // because the latter rejects index >= size, which would break end()/rend() on non-empty lists
    // and both begin()/end() on empty lists.
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::Iterator StableList<T, Allocator>::begin()
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return Iterator(IRef{baseIRef}, this, 1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::begin() const
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return ConstIterator(IRef{baseIRef}, this, 1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::cbegin() const
    {
        return this->begin();
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::Iterator StableList<T, Allocator>::end()
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return Iterator(IRef{baseIRef + FSDS_StableList_size(this->m_list)}, this, 1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::end() const
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return ConstIterator(IRef{baseIRef + FSDS_StableList_size(this->m_list)}, this, 1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::cend() const
    {
        return this->end();
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::Iterator StableList<T, Allocator>::rbegin()
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return Iterator(IRef{baseIRef + FSDS_StableList_size(this->m_list) - 1}, this, -1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::rbegin() const
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return ConstIterator(IRef{baseIRef + FSDS_StableList_size(this->m_list) - 1}, this, -1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::rcbegin() const
    {
        return this->rbegin();
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::Iterator StableList<T, Allocator>::rend()
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return Iterator(IRef{baseIRef - 1}, this, -1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::rend() const
    {
        const size_t baseIRef = this->m_list.memBlock->header.iRefOffset + this->m_list.memBlock->header.front;
        return ConstIterator(IRef{baseIRef - 1}, this, -1);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::rcend() const
    {
        return this->rend();
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::Iterator StableList<T, Allocator>::getIterator(const IRef& start, const int step)
    {
        return Iterator(start, this, step);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::getConstIterator(const IRef& start, const int step) const
    {
        return ConstIterator(start, this, step);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::Iterator StableList<T, Allocator>::getUnorderedIterator()
    {
        return this->begin();
    }
    template<typename T, typename Allocator>
    [[nodiscard]] StableList<T, Allocator>::ConstIterator StableList<T, Allocator>::getUnorderedConstIterator() const
    {
        return this->begin();
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::empty() const noexcept
    {
        return FSDS_StableList_size(this->m_list) == 0;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr bool StableList<T, Allocator>::isEmpty() const noexcept
    {
        return FSDS_StableList_size(this->m_list) == 0;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr size_t StableList<T, Allocator>::size() const noexcept
    {
        return FSDS_StableList_size(this->m_list);
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr size_t StableList<T, Allocator>::maxSize() const noexcept
    {
        return std::numeric_limits<size_t>::max();
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr size_t StableList<T, Allocator>::capacity() const noexcept
    {
        return FSDS_StableList_capacity(this->m_list);
    }

    template<typename T, typename Allocator>
    [[nodiscard]] constexpr size_t StableList<T, Allocator>::iRefToIndex(const IRef& iRef)
    {
        size_t index;
        int retcode = FSDS_StableList_iRefToIndex(this->m_list, iRef.value, &index);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return index;
    }
    template<typename T, typename Allocator>
    [[nodiscard]] constexpr typename StableList<T, Allocator>::IRef StableList<T, Allocator>::indexToIRef(const size_t& index)
    {
        FSDS_StableList_IRef raw;
        int retcode = FSDS_StableList_indexToIRef(this->m_list, index, &raw);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        return IRef{raw};
    }

    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::reserve(size_t newCap)
    {
        int retcode = FSDS_StableList_reserve(&this->m_list, newCap);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::shrinkToFit()
    {
        int retcode = FSDS_StableList_shrinkToFit(&this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::shrink_to_fit()
    {
        int retcode = FSDS_StableList_shrinkToFit(&this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::clear()
    {
        int retcode;
        if constexpr(!std::is_trivially_destructible<T>::value)
        {
            size_t count = FSDS_StableList_size(this->m_list);
            for(size_t i = 0; i < count; i++)
            {
                T* ptr;
                retcode = FSDS_StableList_getElementIndex(this->m_list, i, reinterpret_cast<void**>(&ptr));
                if(retcode)
                {
                    throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
                }
                std::destroy_at(ptr);
            }
        }
        retcode = FSDS_StableList_clear(this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }

    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::append(const T& value)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(value);
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::prepend(const T& value)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendFront(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(value);
    }
    template<typename T, typename Allocator>
    template<typename... Args>
    constexpr void StableList<T, Allocator>::appendConstruct(Args&&... args)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(std::forward<Args>(args)...);
    }
    template<typename T, typename Allocator>
    template<typename... Args>
    constexpr void StableList<T, Allocator>::prependConstruct(Args&&... args)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendFront(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(std::forward<Args>(args)...);
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::push_back(const T& value)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(value);
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::push_front(const T& value)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendFront(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(value);
    }
    template<typename T, typename Allocator>
    template<typename... Args>
    constexpr void StableList<T, Allocator>::emplace_back(Args&&... args)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(std::forward<Args>(args)...);
    }
    template<typename T, typename Allocator>
    template<typename... Args>
    constexpr void StableList<T, Allocator>::emplace_front(Args&&... args)
    {
        T* ptr;
        int retcode = FSDS_StableList_appendFront(&this->m_list, reinterpret_cast<void**>(&ptr));
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
        new(ptr) T(std::forward<Args>(args)...);
    }

    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::concatenateBack(const StableList& other)
    {
        size_t otherSize = FSDS_StableList_size(other.m_list);
        for(size_t i = 0; i < otherSize; i++)
        {
            T* src;
            int retcode = FSDS_StableList_getElementIndex(other.m_list, i, reinterpret_cast<void**>(&src));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            T* dst;
            retcode = FSDS_StableList_appendBack(&this->m_list, reinterpret_cast<void**>(&dst));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(dst) T(*src);
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::concatenateFront(const StableList& other)
    {
        size_t otherSize = FSDS_StableList_size(other.m_list);
        for(size_t i = otherSize; i > 0; i--)
        {
            T* src;
            int retcode = FSDS_StableList_getElementIndex(other.m_list, i - 1, reinterpret_cast<void**>(&src));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            T* dst;
            retcode = FSDS_StableList_appendFront(&this->m_list, reinterpret_cast<void**>(&dst));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
            new(dst) T(*src);
        }
    }
    
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::removeBack()
    {
        int retcode;
        if constexpr(!std::is_trivially_destructible<T>::value)
		{
			T* ptr;
			retcode = FSDS_StableList_getElementIndex(this->m_list, FSDS_StableList_size(this->m_list) - 1, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
			std::destroy_at(ptr);
		}
        retcode = FSDS_StableList_removeBack(this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::removeFront()
    {
        int retcode;
        if constexpr(!std::is_trivially_destructible<T>::value)
		{
			T* ptr;
			retcode = FSDS_StableList_getElementIndex(this->m_list, 0, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
			std::destroy_at(ptr);
		}
        retcode = FSDS_StableList_removeFront(this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::removeBackWithoutDeconstruct()
    {
        int retcode = FSDS_StableList_removeBack(this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::removeFrontWithoutDeconstruct()
    {
        int retcode = FSDS_StableList_removeFront(this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::pop_back()
    {
        int retcode;
        if constexpr(!std::is_trivially_destructible<T>::value)
		{
			T* ptr;
			retcode = FSDS_StableList_getElementIndex(this->m_list, FSDS_StableList_size(this->m_list) - 1, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
			std::destroy_at(ptr);
		}
        retcode = FSDS_StableList_removeBack(this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
    template<typename T, typename Allocator>
    constexpr void StableList<T, Allocator>::pop_front()
    {
        int retcode;
        if constexpr(!std::is_trivially_destructible<T>::value)
		{
			T* ptr;
			retcode = FSDS_StableList_getElementIndex(this->m_list, 0, reinterpret_cast<void**>(&ptr));
            if(retcode)
            {
                throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
            }
			std::destroy_at(ptr);
		}
        retcode = FSDS_StableList_removeFront(this->m_list);
        if(retcode)
        {
            throw std::runtime_error(std::string("Stable list error: ") + FSDS_StableList_errorString(retcode));
        }
    }
}