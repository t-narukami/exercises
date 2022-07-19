#include "Memory.h"
#include "Allocators.h"
#include <iostream>

namespace Memory
{
namespace Private
{
	using GlobalAllocatorType = 
		FallbackAllocator<
			SegregatorAllocator<
				FreelistAllocator<StackAllocator<16_mB>, 112>,
				FreelistAllocator<HeapAllocator<1_gB>, 112>,
				128
			>,
			MallocAllocator
		>;

	static GlobalAllocatorType& GetGlobalAllocator()
	{
		static GlobalAllocatorType globalAllocator;
		return globalAllocator;
	}

	AllocInfo FirstAllocInfo;
	AllocInfo* NextAllocInfo = &FirstAllocInfo;
} // namespace Private

MemDesc Allocate(uint64_t sizeInBytes)
{
	return Private::GetGlobalAllocator().Allocate(sizeInBytes);
}

void Deallocate(MemDesc descriptor)
{
	Private::GetGlobalAllocator().Deallocate(descriptor);
}

void DumpAllocInfo()
{
	Private::AllocInfo* it = Private::FirstAllocInfo.next;
	while (it)
	{
		std::cout << it->filename << ":" << it->line << " " << it->function << "(): " << it->count << " allocations (" << it->totalBytes << " bytes)" << std::endl;
		it = it->next;
	}
}

static inline uint64_t ToMB(uint64_t bytes)
{
	return static_cast<uint64_t>(bytes / (1024. * 1024.));
}

static void PrintStats(Private::AllocatorStatsReportPtr const& ptr, int depth, uint64_t& totalAlloc, uint64_t& totalDealloc)
{
	using namespace std;
	auto const tabs = [] (int tabs) 
	{ 
		for (int i = 0; i < tabs; ++i)
		{
			cout << "    ";
		}
	};

	Private::AllocatorStats const& s = ptr->stats;
	tabs(depth); cout << s.name << endl;
	tabs(depth); cout << "{" << endl;
	if (!ptr->isProxyAllocator)
	{
		tabs(depth + 1); printf("-----------------------------------------\n");
		tabs(depth + 1); printf("|       |  total  |   bytes   |    Mb   |\n");
		tabs(depth + 1); printf("|Alloc  |%9" PRIu64 "|%11" PRIu64 "|%9" PRIu64 "|\n", s.countAllocated, s.totalAllocated, ToMB(s.totalAllocated));
		tabs(depth + 1); printf("|Dealloc|%9" PRIu64 "|%11" PRIu64 "|%9" PRIu64 "|\n", s.countDeallocated, s.totalDeallocated, ToMB(s.totalDeallocated));
		tabs(depth + 1); printf("-----------------------------------------\n");
		totalAlloc += s.totalAllocated;
		totalDealloc += s.totalDeallocated;
	}
	for (auto const& nested : ptr->nested)
	{
		PrintStats(nested, depth + 1, totalAlloc, totalDealloc);
	}
	tabs(depth); cout << "}" << endl;
}

void DumpAllocStats()
{
	Private::AllocatorStatsReportPtr report = std::move(Private::GetGlobalAllocator().GetStats());
	std::cout << "\nALLOCATION STATISTICS" << std::endl;
	uint64_t aTotal = 0;
	uint64_t dTotal = 0;
	PrintStats(report, 0, aTotal, dTotal);
	std::cout << "Total allocated memory:   " << aTotal <<" bytes (" << ToMB(aTotal) << " Mb)" << std::endl;
	std::cout << "Total deallocated memory: " << dTotal <<" bytes (" << ToMB(dTotal) << " Mb)" << std::endl;
	std::cout << "Difference: " << aTotal - dTotal << " bytes" << std::endl;
}

} // namespace Memory