#include "Allocators.h"

#include <cstdlib>

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
	return { std::malloc(size), size };
}

void MallocAllocator::Deallocate(MemDesc desc)
{
	std::free(desc.ptr);
}

} // namespace Memory
