#include <iostream>
#include <string>

#include "DataStructures/Tests.h"
#include "Memory/Tests.h"

#include "DataStructures/BST.h"
#include "DataStructures/BSTv1.h"
#include "Utils/Benchy.h"

void RunBenchmarks();
void RunTests();

int main()
{
	// Trigger memory allocation
	Memory::Deallocate(Memory::Allocate(1));

	//RunTests();
	RunBenchmarks();

	Memory::DumpAllocInfo();
	return 0;
}

void RunTests()
{
	TestMemory();
	TestDataStructures();
}

template <template <typename> class T, typename V>
void BenchBST(std::string const& name)
{
	Benchy::Report report(name);
	int const modulo = 1000000;
	int const count = 2000000;
	for (int i = 0; i < 10; ++i)
	{
		T<V> tree;
		{
			Benchy::Stopwatch sw(report, "Adding 1 million random elements");
			for (int i = 0; i < count; ++i)
			{
				tree.Add({ static_cast<V>(rand() % modulo) });
			}
		}
		{
			Benchy::Stopwatch sw(report, "Looking up random element from 1 million elements");
			tree.Find({ static_cast<V>(rand() % modulo) });
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
	BenchBST<BST, int>("Bench BST<int>");
	BenchBST<BSTv1, int>("Bench BSTv1<int>");
}
