#pragma once
#include <cinttypes>

namespace Memory
{

struct MemDesc
{
	void* ptr = nullptr;
	uint64_t size = 0;
};

} // namespace Memory