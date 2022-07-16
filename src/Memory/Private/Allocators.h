#pragma once
#include "MemDesc.h"
#include "Utils/Assert.h"

namespace Memory
{

class NullAllocator
{
public:
	MemDesc Allocate(uint64_t size);
	void Deallocate(MemDesc desc);
};

class MallocAllocator
{
public:
	MemDesc Allocate(uint64_t size);
	void Deallocate(MemDesc desc);
};

template <typename Primary, typename Fallback>
class FallbackAllocator
{
public:
	FallbackAllocator() = default;
	FallbackAllocator(FallbackAllocator&&) = delete;
	FallbackAllocator(FallbackAllocator const&) = delete;
	FallbackAllocator& operator=(FallbackAllocator&&) = delete;
	FallbackAllocator& operator=(FallbackAllocator const&) = delete;

	MemDesc Allocate(uint64_t size)
	{
		MemDesc primaryDesc = primary.Allocate(size);
		if (!primaryDesc.ptr)
		{
			return fallback.Allocate(size);
		}
		return primaryDesc;
	}

	void Deallocate(MemDesc desc)
	{
		if (primary.Owns(desc))
		{
			primary.Deallocate(desc);
		}
		else
		{
			fallback.Deallocate(desc);
		}
	}

private:
	Primary primary;
	Fallback fallback;
};

template <size_t Size>
class StackAllocator
{
public:
	StackAllocator(StackAllocator&&) = delete;
	StackAllocator(StackAllocator const&) = delete;
	StackAllocator& operator=(StackAllocator&&) = delete;
	StackAllocator& operator=(StackAllocator const&) = delete;

	StackAllocator()
		: ptr(stack)
	{
	}

	MemDesc Allocate(uint64_t size)
	{
		if (Size - (ptr - stack) < size)
		{
			return { nullptr, 0 };
		}
		MemDesc result = { ptr, size };
		ptr += size;
		return result;
	}

	void Deallocate(MemDesc desc)
	{
		MY_ASSERT(Owns(desc), "Stack allocator should own memory you are trying to free");
		if ((ptr - desc.size) == desc.ptr)
		{
			ptr = reinterpret_cast<uint8_t*>(desc.ptr);
		}
	}

	bool Owns(MemDesc desc) const
	{
		return desc.ptr >= stack && desc.ptr < stack + Size;
	}

private:
	uint8_t stack[Size];
	uint8_t* ptr = nullptr;
};

template <size_t Size>
class HeapAllocator
{
public:
	HeapAllocator(HeapAllocator&&) = delete;
	HeapAllocator(HeapAllocator const&) = delete;
	HeapAllocator& operator=(HeapAllocator&&) = delete;
	HeapAllocator& operator=(HeapAllocator const&) = delete;

	HeapAllocator()
		: heap(new uint8_t[Size])
		, ptr(heap)
	{
		MY_ASSERT(heap, "Failed to reserve memory from heap");
	}

	~HeapAllocator()
	{
		delete[] heap;
	}

	MemDesc Allocate(uint64_t size)
	{
		if (Size - (ptr - heap) < size)
		{
			return { nullptr, 0 };
		}
		MemDesc result = { ptr, size };
		ptr += size;
		return result;
	}

	void Deallocate(MemDesc desc)
	{
		MY_ASSERT(Owns(desc), "Heap allocator should own memory you are trying to free");
		if ((ptr - desc.size) == desc.ptr)
		{
			ptr = reinterpret_cast<uint8_t*>(desc.ptr);
		}
	}

	bool Owns(MemDesc desc) const
	{
		return desc.ptr >= heap && desc.ptr < heap + Size;
	}

private:
	uint8_t* heap = nullptr;
	uint8_t* ptr = nullptr;
};

template <typename Allocator, size_t blockSize>
class FreelistAllocator
{
public:
	MemDesc Allocate(uint64_t size)
	{
		if (size == blockSize && list)
		{
			MemDesc result = { list, size };
			list = list->next;
			return result;
		}
		return allocator.Allocate(size);
	}

	void Deallocate(MemDesc desc)
	{
		if (desc.size != blockSize)
		{
			allocator.Deallocate(desc);
		}
		else
		{
			Node* node = reinterpret_cast<Node*>(desc.ptr);
			node->next = list;
			list = node;
		}
	}

	bool Owns(MemDesc desc) const { return desc.size == blockSize || allocator.Owns(desc); }

private:
	Allocator allocator;

	struct Node
	{
		Node* next;
	};
	Node* list = nullptr;
};

template <typename LessOrEqualAllocator, typename GreaterAllocator, size_t Segregator>
class SegregatorAllocator
{
public:
	SegregatorAllocator() = default;

	MemDesc Allocate(uint64_t size)
	{
		if (size <= Segregator)
		{
			return loeAllocator.Allocate(size);
		}
		return greaterAllocator.Allocate(size);
	}

	void Deallocate(MemDesc desc)
	{
		if (desc.size <= Segregator)
		{
			loeAllocator.Deallocate(desc);
		}
		else
		{
			greaterAllocator.Deallocate(desc);
		}
	}

	bool Owns(MemDesc desc) const
	{
		if (desc.size <= Segregator)
		{
			return loeAllocator.Owns(desc);
		}
		else
		{
			return greaterAllocator.Owns(desc);
		}
	}

private:
	LessOrEqualAllocator loeAllocator;
	GreaterAllocator greaterAllocator;
};

} // namespace Memory