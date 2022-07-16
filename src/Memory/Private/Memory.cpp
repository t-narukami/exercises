#include "Memory.h"
#include "Allocators.h"

namespace Memory
{
namespace Private
{
	using GlobalAllocatorType = 
		FallbackAllocator<
			SegregatorAllocator<
				StackAllocator<1_mB>,
				HeapAllocator<1_gB>,
				64
			>,
			MallocAllocator
		>;

	static GlobalAllocatorType& GetGlobalAllocator()
	{
		static GlobalAllocatorType globalAllocator;
		return globalAllocator;
	}
} // namespace Private

MemDesc Allocate(uint64_t sizeInBytes)
{
	return Private::GetGlobalAllocator().Allocate(sizeInBytes);
}

void Deallocate(MemDesc descriptor)
{
	Private::GetGlobalAllocator().Deallocate(descriptor);
}

} // namespace Memory