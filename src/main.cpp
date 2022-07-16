#include <iostream>
#include <string>

#include "DataStructures/Tests.h"
#include "Memory/Tests.h"

#include "DataStructures/BST.h"
#include "Utils/Benchy.h"

void RunBenchmarks();
void RunTests();

int main()
{
	RunTests();
	//RunBenchmarks();

	return 0;
}

void RunTests()
{
	TestMemory();
	TestDataStructures();
}

template <template <typename> typename T, typename V>
void BenchBST(std::string const& name)
{
	Benchy::Report report(name);
	T<V> tree;
	int const modulo = 1000000;
	{
		Benchy::Stopwatch sw(report, "Adding 1 million random elements");
		for (int i = 0; i < 1000000; ++i)
		{
			tree.Add({ static_cast<V>(rand() % modulo) });
		}
	}
	{
		Benchy::Stopwatch sw(report, "Looking up random element from 1 million elements");
		tree.Find({ static_cast<V>(rand() % modulo) });
	}
	{
		Benchy::Stopwatch sw(report, "Erasing 1 million elements");
		for (int i = 0; i < modulo; ++i)
		{
			tree.Erase({ static_cast<V>(i) });
		}
	}
}

void RunBenchmarks()
{
	BenchBST<BST, int>("Bench BST<int>");
	//BenchBST<AVLTree, int>("Bench AVLTree<int>");
}
