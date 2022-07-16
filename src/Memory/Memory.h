#pragma once
#include "MemDesc.h"

constexpr std::uint64_t operator""_kB(uint64_t v) { return v << 10; }
constexpr std::uint64_t operator""_mB(uint64_t v) { return v << 20; }
constexpr std::uint64_t operator""_gB(uint64_t v) { return v << 30; }

namespace Memory
{

MemDesc Allocate(uint64_t sizeInBytes);

void Deallocate(MemDesc descriptor);

} // namespace Memory