#include "Tests.h"
#include "Utils/Testy.h"

#include "MemDesc.h"
#include "Allocators.h"

using namespace Memory;

void TestMemDesc()
{
	TEST("Test MemDesc");

	MemDesc d;
	ASSERT(d.ptr == nullptr, "MemDesc ptr should be nullptr by default");
	ASSERT(d.size == 0, "MemDesc size should be 0 by default");

	int value = 10;
	d.ptr = &value;
	d.size = sizeof(value);

	MemDesc otherD = d;
	ASSERT(otherD.ptr == d.ptr && otherD.size == d.size, "MemDesc can be copied");
}

template <template <size_t> class Allocator>
void TestLinearAllocator(std::string const& name)
{
	TEST(name);

	Allocator<64> ator;
	MemDesc eightBytes = ator.Allocate(8);
	ASSERT(eightBytes.ptr, "Can allocate 8 of 64 bytes");
	MemDesc fiftysixBytes = ator.Allocate(56);
	ASSERT(fiftysixBytes.ptr, "Can allocate 64 of 64 bytes");
	MemDesc noMoreBytes = ator.Allocate(1);
	ASSERT(noMoreBytes.ptr == nullptr, "Can't allocate more than 64 bytes");
	ator.Deallocate(fiftysixBytes);
	MemDesc sixteenBytes = ator.Allocate(16);
	ASSERT(sixteenBytes.ptr, "Can deallocate last block of memory");
}

struct TestAllocator
{
	static int counter;
	MemDesc Allocate(uint64_t)
	{
		++counter;
		return { nullptr, 0 };
	}

	void Deallocate(MemDesc desc)
	{
		--counter;
	}
};
int TestAllocator::counter = 0;

void TestFallbackAllocator()
{
	TEST("Test FallbackAllocator");

	FallbackAllocator<StackAllocator<64>, TestAllocator> ator;
	ASSERT(TestAllocator::counter == 0, "No allocations at construction");

	MemDesc primDesc =  ator.Allocate(32);
	ASSERT(TestAllocator::counter == 0, "Should allocate from primary allocator");
	MemDesc fallbackDesc = ator.Allocate(64);
	ASSERT(TestAllocator::counter == 1, "Should allocate from fallback allocator");
	ator.Allocate(32);
	ASSERT(TestAllocator::counter == 1, "Should allocate from primary allocator");
	ator.Deallocate(primDesc);
	ASSERT(TestAllocator::counter == 1, "Should deallocate from primary allocator");
	ator.Deallocate(fallbackDesc);
	ASSERT(TestAllocator::counter == 0, "Should deallocate from fallback allocator");
}

static int testStackAllocatorCounter1 = 0;
static int testStackAllocatorCounter2 = 0;

template <size_t Size>
struct TestStackAllocator1
{
	MemDesc Allocate(uint64_t size)
	{
		++testStackAllocatorCounter1;
		return allocator.Allocate(size);
	}

	void Deallocate(MemDesc desc)
	{
		--testStackAllocatorCounter1;
		allocator.Deallocate(desc);
	}

	bool Owns(MemDesc desc) const { return allocator.Owns(desc); }
private:
	StackAllocator<Size> allocator;
};

template <size_t Size>
struct TestStackAllocator2
{
	MemDesc Allocate(uint64_t size)
	{
		++testStackAllocatorCounter2;
		return allocator.Allocate(size);
	}

	void Deallocate(MemDesc desc)
	{
		--testStackAllocatorCounter2;
		allocator.Deallocate(desc);
	}

	bool Owns(MemDesc desc) const { return allocator.Owns(desc); }
private:
	StackAllocator<Size> allocator;
};

void TestSegregatorAllocator()
{
	TEST("Test SegregatorAllocator");

	SegregatorAllocator<TestStackAllocator1<64>, TestStackAllocator2<64>, 32> ator;
	ASSERT(testStackAllocatorCounter1 == 0 && testStackAllocatorCounter2 == 0, "No allocations at construction");

	MemDesc eightBytes = ator.Allocate(8);
	ASSERT(eightBytes.ptr && testStackAllocatorCounter1 == 1, "Should use first allocator");
	MemDesc thirtyTwoBytes = ator.Allocate(32);
	ASSERT(thirtyTwoBytes.ptr && testStackAllocatorCounter2 == 1, "Should use second allocator");
	ator.Deallocate(eightBytes);
	ASSERT(eightBytes.ptr && testStackAllocatorCounter1 == 0, "Should use first allocator");
	ator.Deallocate(thirtyTwoBytes);
	ASSERT(thirtyTwoBytes.ptr && testStackAllocatorCounter2 == 0, "Should use second allocator");
}

void TestFreelistAllocator()
{
	TEST("Test FreelistAllocator");
	testStackAllocatorCounter1 = 0;

	FreelistAllocator<TestStackAllocator1<64>, 8> ator;
	ASSERT(testStackAllocatorCounter1 == 0, "No allocations at construction");

	MemDesc eightBytes = ator.Allocate(8);
	ASSERT(eightBytes.ptr && testStackAllocatorCounter1 == 1, "Allocate from the allocator when list is empty");

	ator.Deallocate(eightBytes);
	ASSERT(testStackAllocatorCounter1 == 1, "Deallocate to the free list when the size match");

	MemDesc sixteenBytes = ator.Allocate(16);
	ASSERT(sixteenBytes .ptr && testStackAllocatorCounter1 == 2, "Allocate from the allocator when the size don't match");

	eightBytes = ator.Allocate(8);
	ASSERT(eightBytes.ptr && testStackAllocatorCounter1 == 2, "Allocate from the free list");

	MemDesc anotherEightBytes = ator.Allocate(8);
	ASSERT(anotherEightBytes.ptr && testStackAllocatorCounter1 == 3, "Allocate from the allocator whne list is empty");

	ator.Deallocate(sixteenBytes);
	ASSERT(testStackAllocatorCounter1 == 2, "Deallocate from the allocator when the size don't match");

	ator.Deallocate(anotherEightBytes);
	ASSERT(testStackAllocatorCounter1 == 2, "Deallocate to the free list");
}

void TestMemory()
{
	TestMemDesc();
	TestLinearAllocator<StackAllocator>("Tets StackAllocator");
	TestLinearAllocator<HeapAllocator>("Tets HeapAllocator");
	TestFallbackAllocator();
	TestSegregatorAllocator();
	TestFreelistAllocator();
}