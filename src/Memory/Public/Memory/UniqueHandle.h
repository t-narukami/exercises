#pragma once
#include "Assert.h"
#include "Memory.h"

namespace Memory
{

template <typename T>
class UniqueHandle
{
public:
	UniqueHandle() = default;
	UniqueHandle(nullptr_t) {};
	UniqueHandle(UniqueHandle const&) = delete;
	UniqueHandle& operator=(UniqueHandle const&) = delete;

	~UniqueHandle()
	{
		if (IsValid())
		{
			Get()->~T();
			Deallocate(m_desc);
		}
	}

	UniqueHandle(UniqueHandle&& other) noexcept
	{
		std::swap(m_desc, other.m_desc);
	}

	UniqueHandle& operator=(UniqueHandle&& other) noexcept
	{
		std::swap(m_desc, other.m_desc);
		return *this;
	}

	bool IsValid() const { return m_desc.ptr; }

	operator bool() const { return IsValid(); }

	T& operator*() { return *Get(); }
	T* operator->() { return Get(); }

	T const& operator*() const { return *Get(); }
	T const* operator->() const { return Get(); }

private:
	template <typename T, typename ...Args>
	friend UniqueHandle<T> MakeUnique(Args&& ...args);

	UniqueHandle(MemDesc desc)
		: m_desc(desc)
	{
	}

	T* Get() 
	{
		MY_ASSERT(IsValid(), "Dereferencing invalid UniqueHandle");
		return reinterpret_cast<T*>(m_desc.ptr);
	}

	T const* Get() const 
	{
		MY_ASSERT(IsValid(), "Dereferencing invalid UniqueHandle");
		return reinterpret_cast<T const*>(m_desc.ptr); 
	}

	MemDesc m_desc;
};

template <typename T, typename ...Args>
UniqueHandle<T> MakeUnique(Args&& ...args)
{
	MemDesc desc = Allocate(sizeof(T));
	MY_ASSERT(desc.ptr, "Failed to allocate memory");

	new (desc.ptr) T(std::forward<Args>(args)...);

	return { desc };
}

} // namespace Memory