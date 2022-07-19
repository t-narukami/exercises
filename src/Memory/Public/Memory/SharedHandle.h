#pragma once
#include "Assert.h"
#include "Memory.h"

namespace Memory
{

template <typename T>
class SharedHandle
{
public:
	SharedHandle() = default;

	~SharedHandle()
	{
		Decrement();
	}

	SharedHandle(SharedHandle const& other)
		: m_desc(other.m_desc)
		, m_refCounter(other.m_refCounter)
		, m_dataPtr(other.m_dataPtr)
	{
		Increment();
	}

	SharedHandle& operator=(SharedHandle const& other)
	{
		Decrement();
		m_desc = other.m_desc;
		m_refCounter = other.m_refCounter;
		m_dataPtr = other.m_dataPtr;
		Increment();
		return *this;
	}

	void Swap(SharedHandle&& other) noexcept
	{
		std::swap(m_desc, other.m_desc);
		std::swap(m_refCounter, other.m_refCounter);
		std::swap(m_dataPtr, other.m_dataPtr);
	}

	SharedHandle(SharedHandle&& other) noexcept
	{
		this->Swap(std::move(other));
	}

	SharedHandle& operator=(SharedHandle&& other) noexcept
	{
		this->Swap(std::move(other));
		return *this;
	}

	bool IsValid() const { return m_refCounter && m_dataPtr; }

	operator bool() const { return IsValid(); }

	T& operator*() { return *Get(); }
	T* operator->() { return Get(); }

	T const& operator*() const { return *Get(); }
	T const* operator->() const { return Get(); }

private:
	template <typename T, typename ...Args>
	friend SharedHandle<T> MakeShared(Args&& ...args);

	using RefCounter = uint64_t;

	SharedHandle(MemDesc desc, void* dataPtr)
		: m_desc(desc)
		, m_refCounter(reinterpret_cast<RefCounter*>(desc.ptr))
		, m_dataPtr(dataPtr)
	{
		if (IsValid())
		{
			*m_refCounter = 1;
		}
	}

	void Increment()
	{
		if (IsValid())
		{
			++*m_refCounter;
		}
	}

	void Decrement()
	{
		if (IsValid())
		{
			if (*m_refCounter)
			{
				--*m_refCounter;
			}
			if (*m_refCounter == 0)
			{
				ReleaseReset();
			}
		}
	}

	void ReleaseReset()
	{
		reinterpret_cast<T*>(m_dataPtr)->~T();
		Deallocate(m_desc);
		m_desc.ptr = nullptr;
		m_dataPtr = nullptr;
		m_refCounter = nullptr;
	}

	inline T* Get() 
	{
		//MY_ASSERT(IsValid(), "Dereferencing invalid SharedHandle");
		return reinterpret_cast<T*>(m_dataPtr);
	}

	inline T const* Get() const 
	{
		//MY_ASSERT(IsValid(), "Dereferencing invalid SharedHandle");
		return reinterpret_cast<T const*>(m_dataPtr); 
	}

	MemDesc m_desc;
	RefCounter* m_refCounter = nullptr;
	void* m_dataPtr = nullptr;
};

template <typename T, typename ...Args>
SharedHandle<T> MakeShared(Args&& ...args)
{
	const uint64_t dataPtrOffset = sizeof(SharedHandle<T>::RefCounter);
	MemDesc desc = ALLOCATE(sizeof(T) + dataPtrOffset);
	MY_ASSERT(desc.ptr, "Failed to allocate memory");

	void* dataPtr = new (reinterpret_cast<uint8_t*>(desc.ptr) + dataPtrOffset) T(std::forward<Args>(args)...);

	return { desc, dataPtr };
}

} // namespace Memory