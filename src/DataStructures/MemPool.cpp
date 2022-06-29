#include "MemPool.h"
#include "Assert.h"

namespace Memory
{

MemPool::MemPool(size_t size)
	: m_memory(std::make_unique<uint8_t[]>(size))
{
	MY_ASSERT(m_memory, "Failed to allocate memory pool");

	FrameDesc* first = new (m_memory.get()) FrameDesc();

	// Last descriptor is invalid by default
	first->next = new (m_memory.get() + (size - sizeof FrameDesc)) FrameDesc();
}

MemPool::~MemPool()
{
	if (!m_memory)
	{
		return;
	}
	FrameDesc* it = GetFrameDesc(m_memory.get());
	while (it->IsValid())
	{
		if (!it->IsFree())
		{
			it->dtor(GetObjPtr(it));
		}
		it = it->next;
	}
}

MemPool::FrameDesc* MemPool::GetFrameDesc(void* ptr) const
{
	return reinterpret_cast<FrameDesc*>(ptr);
}

void MemPool::Increment(void* ptr)
{
	MY_ASSERT(ptr, "Invalid frame pointer");
	FrameDesc* desc = GetFrameDesc(ptr);
	MY_ASSERT(desc->IsValid(), "Incrementing invalid frame descriptor");
	++desc->refs;
}

void MemPool::Decrement(void* ptr)
{
	FrameDesc* desc = GetFrameDesc(ptr);
	MY_ASSERT(desc->IsValid(), "Decrementing invalid frame descriptor");
	MY_ASSERT(!desc->IsFree(), "Decrementing free frame descriptor");
	--desc->refs;
	if (desc->IsFree())
	{
		desc->dtor(GetObjPtr(desc));

		if (desc->next->IsFree())
		{
			desc->next = desc->next->next;
		}

		FrameDesc* it = GetFrameDesc(m_memory.get());
		if (it != desc)
		{
			while (it->next != desc)
			{
				it = it->next;
			}
			if (it->IsFree())
			{
				it->next = desc->next;
			}
		}
	}
}

MemPool::FrameDesc* MemPool::GetAvailableFrame(size_t requiredSize)
{
	FrameDesc* ptr = GetFrameDesc(m_memory.get());
	while (ptr->IsValid() && !(ptr->IsFree() && ptr->Size() >= requiredSize))
	{
		ptr = ptr->next;
	}
	return ptr;
}

} // namespace Memory