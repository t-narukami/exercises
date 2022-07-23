#include <iostream>
#include <string>
#include <random>

#include "DataStructures/Tests.h"
#include "Memory/Tests.h"

#include "DataStructures/BST.h"
#include "DataStructures/BSTv1.h"
#include "Utils/Benchy.h"
#include "Memory/Memory.h"

void RunBenchmarks();
void RunTests();

int main()
{
	// Trigger memory allocation
	//Memory::Deallocate(Memory::Allocate(1));

	RunTests();
	//RunBenchmarks();

	Memory::DumpAllocInfo();
	Memory::DumpMemoryUsage();
	return 0;
}

void RunTests()
{
	TestMemory();
	TestDataStructures();
}

template <template <typename> class T, typename V>
void BenchBST(std::string const& name, std::random_device& rd)
{
	Benchy::Report report(name);
	int const modulo = 1000000;
	int const count = 1000000;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, modulo);

	for (int i = 0; i < 10; ++i)
	{
		T<V> tree;
		{
			Benchy::Stopwatch sw(report, "Adding 1 million random elements");
			for (int i = 0; i < count; ++i)
			{
				tree.Add({ static_cast<V>(dist(gen)) });
			}
		}
		{
			Benchy::Stopwatch sw(report, "Looking up random element from 1 million elements");
			tree.Find({ static_cast<V>(dist(gen)) });
		}
		{
			T<V> copy;
			{
				Benchy::Stopwatch sw(report, "Copying 1 million elements tree");
				copy = tree;
			}
		}
		{
			Benchy::Stopwatch sw(report, "Erasing 1 million elements");
			for (int i = 0; i < count; ++i)
			{
				tree.Erase({ static_cast<V>(i) });
			}
		}
	}
}

void RunBenchmarks()
{
	std::random_device rd;
	BenchBST<BST, int>("Bench BST<int>", rd);
	BenchBST<BSTv1, int>("Bench BSTv1<int>", rd);
}
