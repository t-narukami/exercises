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
				StackAllocator<16_mB>,
				HeapAllocator<1_gB>,
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

} // namespace Memory