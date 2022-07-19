#include "Allocators.h"

#include <cstdlib>
#include <iostream>

namespace Memory
{

MemDesc NullAllocator::Allocate(uint64_t size)
{
	return { nullptr, 0 };
}

void NullAllocator::Deallocate(MemDesc desc)
{
}

MemDesc MallocAllocator::Allocate(uint64_t size)
{
	Private::AddAllocationStat(m_stats, size);
	return { std::malloc(size), size };
}

void MallocAllocator::Deallocate(MemDesc desc)
{
	Private::AddDeallocateStat(m_stats, desc.size);
	std::free(desc.ptr);
}

} // namespace Memory
