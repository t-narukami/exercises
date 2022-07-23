#pragma once
#include "MemDesc.h"

constexpr std::uint64_t operator""_kB(uint64_t v) { return v << 10; }
constexpr std::uint64_t operator""_mB(uint64_t v) { return v << 20; }
constexpr std::uint64_t operator""_gB(uint64_t v) { return v << 30; }

namespace Memory
{

namespace Private
{
	struct AllocInfo
	{
		AllocInfo* next = nullptr;
		const char* filename = nullptr;
		const char* function = nullptr;
		long line = 0;
		uint64_t count = 0;
		uint64_t totalBytes = 0;
	};

	extern AllocInfo* NextAllocInfo;
} // namespace Private

//#define __ENABLE_ALLOCINFO

#ifdef __ENABLE_ALLOCINFO

#if defined ( WIN32 )
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define ALLOCATE(sizeInBytes)\
	Memory::Allocate(sizeInBytes);\
	{\
		static Memory::Private::AllocInfo info;\
		if (info.count == 0) {\
			info.filename = __FILE__;\
			info.function = __PRETTY_FUNCTION__;\
			info.line = __LINE__;\
			Memory::Private::NextAllocInfo->next = &info;\
			Memory::Private::NextAllocInfo = &info;\
		}\
		++info.count;\
		info.totalBytes += sizeInBytes;\
	}
#else
#define ALLOCATE(sizeInBytes) Memory::Allocate(sizeInBytes);
#endif

MemDesc Allocate(uint64_t sizeInBytes);

void Deallocate(MemDesc descriptor);

void DumpAllocInfo();

void DumpMemoryUsage();

} // namespace Memory