#pragma once
#include <memory>

template <typename T>
class ObjectPool
{
public:
	ObjectPool() = default;

	~ObjectPool()
	{
		Clear();
	}

	ObjectPool(ObjectPool const& rhs)
		: m_data(std::make_unique<uint8_t[]>(rhs.Count() * sizeof T))
		, m_capacity(rhs.Count())
		, m_count(rhs.Count())
	{
		for (size_t i = 0; i < m_count; ++i)
		{
			new (m_data.get() + i * sizeof T) T(rhs.At(i));
		}
	}

	ObjectPool(ObjectPool&& rhs)
		: m_data(std::move(rhs.m_data))
		, m_capacity(rhs.m_capacity)
		, m_count(rhs.m_count)
	{
	}

	ObjectPool& operator=(ObjectPool const& rhs)
	{
		m_data = std::make_unique<uint8_t[]>(rhs.Count() * sizeof T);
		m_count = rhs.m_count;
		m_capacity = rhs.m_capacity;

		for (size_t i = 0; i < m_count; ++i)
		{
			new (m_data.get() + i * sizeof T) T(rhs.At(i));
		}
		return *this;
	}

	ObjectPool& operator=(ObjectPool&& rhs)
	{
		m_data = std::move(rhs.m_data);
		m_capacity = rhs.m_capacity;
		m_count = rhs.m_count;
		return *this;
	}

	size_t Count() const { return m_count; }

	T& Add(T const& v)
	{
		if ((m_count + 1) > m_capacity)
		{
			Resize((m_capacity + 1) * 2);
		}
		T* ptr = new (m_data.get() + m_count * sizeof T) T(v);
		m_count++;
		return *ptr;
	}

	T& Add(T&& v)
	{
		if ((m_count + 1) > m_capacity)
		{
			Resize((m_capacity + 1) * 2);
		}
		T* ptr = new(m_data.get() + m_count * sizeof T) T(std::move(v));
		m_count++;
		return *ptr;
	}

	template <typename ...Args>
	T& Emplace(Args&& ...args)
	{
		if ((m_count + 1) > m_capacity)
		{
			Resize((m_capacity + 1) * 2);
		}
		T* ptr = new(m_data.get() + m_count * sizeof T) T(std::forward<Args>(args)...);
		m_count++;
		return *ptr;
	}

	void Erase(size_t idx)
	{
		At(idx).~T();

		size_t const nextElementsCount = m_count - (idx + 1);
		if (nextElementsCount)
		{
			std::memmove(m_data.get() + idx * sizeof T, m_data.get() + (idx + 1) * sizeof T, nextElementsCount * sizeof T);
		}
		m_count--;
	}

	void Clear()
	{
		for (size_t i = 0; i < Count(); ++i)
		{
			At(i).~T();
		}
		m_count = 0;
	}

	T& At(size_t idx)
	{
		assert(idx < m_count && "Index out of bounds");
		return *reinterpret_cast<T*>(m_data.get() + idx * sizeof T);
	}

	T const& At(size_t idx) const
	{
		assert(idx < m_count && "Index out of bounds");
		return *reinterpret_cast<T const*>(m_data.get() + idx * sizeof T);
	}

	void Reserve(size_t maxCount)
	{
		if (m_capacity < maxCount)
		{
			Resize(maxCount);
		}
	}

private:
	void Resize(size_t newCapacity)
	{
		auto newData = std::make_unique<uint8_t[]>(sizeof T * newCapacity);
		std::memcpy(newData.get(), m_data.get(), sizeof T * m_count);
		m_data = std::move(newData);
		m_capacity = newCapacity;
	}

	std::unique_ptr<uint8_t[]> m_data = nullptr;
	size_t m_capacity = 0;
	size_t m_count = 0;
};