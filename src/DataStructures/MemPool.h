#pragma once
#include "Assert.h"
#include <cinttypes>
#include <memory>

constexpr std::size_t operator""_kB(uint64_t v) { return v << 10; }
constexpr std::size_t operator""_mB(uint64_t v) { return v << 20; }
constexpr std::size_t operator""_gB(uint64_t v) { return v << 30; }

namespace Memory
{

class MemPool;

template <typename T>
class Handle
{
public:
	Handle() = default;

	Handle(Handle const& rhs)
		: m_pool(rhs.m_pool)
		, m_ptr(rhs.m_ptr)
	{
		Acquire();
	}

	Handle& operator=(Handle const& rhs)
	{
		Release();
		m_pool = rhs.m_pool;
		m_ptr = rhs.m_ptr;
		Acquire();
		return *this;
	}

	Handle(Handle&& rhs)
	{
		swap(std::move(rhs));
	}

	Handle& operator=(Handle&& rhs)
	{
		swap(std::move(rhs));
		return *this;
	}

	~Handle()
	{
		Release();
	}

	void swap(Handle&& rhs)
	{
		std::swap(m_pool, rhs.m_pool);
		std::swap(m_ptr, rhs.m_ptr);
	}

	bool IsValid() const { return m_pool && m_ptr; }

	operator bool() const { return IsValid(); }

	T& operator*() { MY_ASSERT(IsValid(), "Dereferencing invalid handle"); return *m_pool->Get<T>(m_ptr); }
	T* operator->() { MY_ASSERT(IsValid(), "Accessing invalid handle"); return m_pool->Get<T>(m_ptr); }

	T const& operator*() const { return this->operator*(); }
	T const* operator->() const { return this->operator->(); }
private:
	friend class MemPool;

	Handle(MemPool* pool, void* ptr)
		: m_pool(pool)
		, m_ptr(ptr)
	{
		Acquire();
	}

	void Acquire() const { if (IsValid()) m_pool->Increment(m_ptr); }
	void Release() const { if (IsValid()) m_pool->Decrement(m_ptr); }

	MemPool* m_pool = nullptr;
	void* m_ptr = nullptr;
};


class MemPool
{
public:
	MemPool() = delete;
	MemPool(MemPool const&) = delete;
	MemPool(MemPool&& rhs) = delete;
	MemPool& operator=(MemPool const&) = delete;
	MemPool& operator=(MemPool&&) = delete;

	MemPool(size_t size);
	~MemPool();

	template <typename T, typename ...Args>
	Handle<T> Alloc(Args&& ...args)
	{
		size_t const frameSize = sizeof(T) + sizeof FrameDesc;
		FrameDesc* desc = GetAvailableFrame(frameSize);
		MY_ASSERT(desc->IsValid(), "No available frames left");
		MY_ASSERT(desc->IsFree(), "Found occupied frame for some reason");

		new (GetObjPtr(desc)) T(std::forward<Args>(args)...);
		desc->dtor = [](void const* ptr) { static_cast<T const*>(ptr)->~T(); };

		// If we can fit one more FrameDesc and we have space left then insert it
		if (desc->Size() > frameSize + sizeof FrameDesc)
		{
			FrameDesc* newDesc = new (reinterpret_cast<uint8_t*>(desc) + frameSize) FrameDesc();
			newDesc->next = desc->next;
			desc->next = newDesc;
		}

		return { this, desc };
	}

protected:
	template <typename T>
	friend class Handle;

	void Increment(void* ptr);
	void Decrement(void* ptr);

	template <typename T>
	T* Get(void* ptr)
	{
		FrameDesc* desc = GetFrameDesc(ptr);
		MY_ASSERT(desc->IsValid(), "Trying to get invalid frame");
		MY_ASSERT(!desc->IsFree(), "Trying to get free frame somehow");
		return reinterpret_cast<T*>(GetObjPtr(desc));
	}

private:
	struct FrameDesc
	{
		FrameDesc* next = nullptr;
		uint8_t refs = 0;
		void(*dtor)(void const*) = nullptr;

		bool IsFree() const { return !refs; }
		bool IsValid() const { return next; }
		uint32_t Size() const { return static_cast<uint32_t>(reinterpret_cast<uint8_t const*>(next) - reinterpret_cast<uint8_t const*>(this)); }
	};

	FrameDesc* GetFrameDesc(void* ptr) const;
	FrameDesc* GetAvailableFrame(size_t requiredSize);
	void* GetObjPtr(FrameDesc* desc) { return desc + 1; }

	std::unique_ptr<uint8_t[]> m_memory = nullptr;
};

} // namespace Memory