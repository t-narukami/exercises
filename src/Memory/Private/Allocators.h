#pragma once
#include "MemDesc.h"
#include "Utils/Assert.h"

#include <memory>
#include <vector>

namespace Memory
{

#define __ENABLE_ALLOCATOR_STATS

namespace Private
{
	struct AllocatorStats
	{
		AllocatorStats() = delete;
		AllocatorStats(const char* n) : name(n) {}

		const char* name = nullptr;
		uint64_t countAllocated = 0;
		uint64_t totalAllocated = 0;
		uint64_t countDeallocated = 0;
		uint64_t totalDeallocated = 0;
	};

	inline void AddAllocationStat(AllocatorStats& s, uint64_t size)
	{
#ifdef __ENABLE_ALLOCATOR_STATS
		++s.countAllocated; 
		s.totalAllocated += size;
#elif
		(void)s;(void)size;
#endif
	}
	inline void AddDeallocateStat(AllocatorStats& s, uint64_t size)
	{
#ifdef __ENABLE_ALLOCATOR_STATS
		++s.countDeallocated;
		s.totalDeallocated += size;
#elif
		(void)s;(void)size;
#endif
	}

	struct AllocatorStatsReport
	{
		bool isProxyAllocator = false;
		AllocatorStats stats = "";
		std::vector<std::unique_ptr<AllocatorStatsReport>> nested;
	};
	using AllocatorStatsReportPtr = std::unique_ptr<AllocatorStatsReport>;
}

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

	Private::AllocatorStatsReportPtr GetStats() const
	{
		auto report = std::make_unique<Private::AllocatorStatsReport>();
		report->stats = m_stats;
		return std::move(report);
	}
private:
	Private::AllocatorStats m_stats = "MallocAllocator";
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
		Private::AddAllocationStat(m_stats, size);
		MemDesc result = { ptr, size };
		ptr += size;
		return result;
	}

	void Deallocate(MemDesc desc)
	{
		MY_ASSERT(Owns(desc), "Stack allocator should own memory you are trying to free");
		Private::AddDeallocateStat(m_stats, desc.size);
		if ((ptr - desc.size) == desc.ptr)
		{
			ptr = reinterpret_cast<uint8_t*>(desc.ptr);
		}
	}

	bool Owns(MemDesc desc) const
	{
		return desc.ptr >= stack && desc.ptr < stack + Size;
	}

	Private::AllocatorStatsReportPtr GetStats() const
	{
		auto report = std::make_unique<Private::AllocatorStatsReport>();
		report->stats = m_stats;
		return std::move(report);
	}
private:
	uint8_t stack[Size];
	uint8_t* ptr = nullptr;
	Private::AllocatorStats m_stats = "StackAllocator";
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
		Private::AddAllocationStat(m_stats, size);
		MemDesc result = { ptr, size };
		ptr += size;
		return result;
	}

	void Deallocate(MemDesc desc)
	{
		MY_ASSERT(Owns(desc), "Heap allocator should own memory you are trying to free");
		Private::AddDeallocateStat(m_stats, desc.size);
		if ((ptr - desc.size) == desc.ptr)
		{
			ptr = reinterpret_cast<uint8_t*>(desc.ptr);
		}
	}

	bool Owns(MemDesc desc) const
	{
		return desc.ptr >= heap && desc.ptr < heap + Size;
	}

	Private::AllocatorStatsReportPtr GetStats() const
	{
		auto report = std::make_unique<Private::AllocatorStatsReport>();
		report->stats = m_stats;
		return std::move(report);
	}
private:
	uint8_t* heap = nullptr;
	uint8_t* ptr = nullptr;
	Private::AllocatorStats m_stats = "HeapAllocator";
};

template <typename Allocator, size_t blockSize>
class FreelistAllocator
{
public:
	MemDesc Allocate(uint64_t size)
	{
		if (size == blockSize && list)
		{
			Private::AddAllocationStat(m_stats, size);
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
			Private::AddDeallocateStat(m_stats, desc.size);
			Node* node = reinterpret_cast<Node*>(desc.ptr);
			node->next = list;
			list = node;
		}
	}

	bool Owns(MemDesc desc) const { return desc.size == blockSize || allocator.Owns(desc); }

	Private::AllocatorStatsReportPtr GetStats() const
	{
		auto report = std::make_unique<Private::AllocatorStatsReport>();
		report->stats = m_stats;
		report->nested.push_back(std::move(allocator.GetStats()));
		return std::move(report);
	}
private:
	Allocator allocator;

	struct Node
	{
		Node* next;
	};
	Node* list = nullptr;
	Private::AllocatorStats m_stats = "FreelistAllocator";
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

	Private::AllocatorStatsReportPtr GetStats() const
	{
		auto report = std::make_unique<Private::AllocatorStatsReport>();
		report->stats.name = "FallbackAllocator";
		report->isProxyAllocator = true;
		report->nested.push_back(std::move(primary.GetStats()));
		report->nested.push_back(std::move(fallback.GetStats()));
		return std::move(report);
	}
private:
	Primary primary;
	Fallback fallback;
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

	Private::AllocatorStatsReportPtr GetStats() const
	{
		auto report = std::make_unique<Private::AllocatorStatsReport>();
		report->stats.name = "SegregatorAllocator";
		report->isProxyAllocator = true;
		report->nested.push_back(std::move(loeAllocator.GetStats()));
		report->nested.push_back(std::move(greaterAllocator.GetStats()));
		return std::move(report);
	}

private:
	LessOrEqualAllocator loeAllocator;
	GreaterAllocator greaterAllocator;
};
} // namespace Memory