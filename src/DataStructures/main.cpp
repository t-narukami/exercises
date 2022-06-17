#include <iostream>
#include <cassert>
#include <string>

#include "ObjectPool.h"

void TestObjectPool()
{
	ObjectPool<std::string> pool;
	assert(pool.Count() == 0 && "Construct and empty pool of strings");
	pool.Add("first");
	assert(pool.Count() == 1 && "Add an element");
	assert(pool.At(0) == "first" && "Element is correct");
	pool.Emplace("second");
	assert(pool.Count() == 2 && "Emplace another element");
	assert(pool.At(1) == "second" && "Another element is also correct");
	pool.Erase(0);
	assert(pool.Count() == 1 && "Remove first element");
	assert(pool.At(0) == "second" && "Second element is now first");
	pool.Clear();
	assert(pool.Count() == 0 && "Clear the pool");

	for (int i = 0; i < 10; ++i)
	{
		pool.Add(std::to_string(i));
	}
	assert(pool.Count() == 10 && "Add 10 elemets");

	for (int i = 0; i < pool.Count(); ++i)
	{
		assert(pool.At(i) == std::to_string(i) && "All the elements are correct");
	}

	ObjectPool<std::string> anotherPool = pool;
	assert(anotherPool.Count() == pool.Count() && "Copy the pool");

	for (int i = 0; i < pool.Count(); ++i)
	{
		assert(anotherPool.At(i) == pool.At(i) && "Correct elements are copied");
	}

	anotherPool.Erase(anotherPool.Count() - 1);
	assert(anotherPool.Count() == 9 && "Erase last element");

	{
		struct MovableObj
		{
			explicit MovableObj(int v) : value(v) {}
			MovableObj(MovableObj const&) = delete;
			MovableObj(MovableObj&& rhs) noexcept : value(rhs.value) {};

			int value = 0;
		};

		ObjectPool<MovableObj> data;
		data.Add(MovableObj(10));
		assert(data.At(0).value == 10 && "Add with move constructor");
	}
	{
		struct CopyableObj
		{
			explicit CopyableObj(int v) : value(v) {}
			CopyableObj(CopyableObj const& rhs) : value(rhs.value) {};
			CopyableObj(CopyableObj&& rhs) = delete;

			int value = 0;
		};

		ObjectPool<CopyableObj> data;
		CopyableObj obj(10);
		data.Add(obj);
		assert(data.At(0).value == 10 && "Add with move constructor");
	}
	{
		struct NonTrivialObj
		{
			NonTrivialObj() = delete;
			explicit NonTrivialObj(int v) : value(v) {}

			int value = 0;
		};

		ObjectPool<NonTrivialObj> data;
		data.Add(NonTrivialObj(10));
		assert(data.At(0).value == 10 && "Add with move constructor");
		NonTrivialObj obj(11);
		data.Add(obj);
		assert(data.At(1).value == 11 && "Add with copy constructor");
		data.Emplace(12);
		assert(data.At(2).value == 12 && "Emplace with arguments");
	}

	std::cout << __FUNCTION__ << ": PASSED!" << std::endl;
}

void TestAVLTree()
{
	/*
	AVLTree<int> tree;
	assert(tree.Count() == 0 && "Construct empty tree");

	tree.Add(10);
	assert(tree.Count() == 1 && "Add one element");
	assert(*tree.Begin() == 10 && "Element is at the start");
	assert(tree.Find(10) != nullptr && "Element can be searched");
	assert(*tree.Find(10) == 10 && "Element can be searched and value is correct");

	tree.Add(10);
	assert(tree.Count() == 2 && "Add the same element");
	assert(*tree.Find(10) == 10 && "Element is searched correctly");
	tree.Erase(tree.Find(10));
	assert(tree.Count() == 1 && "Element is delete by iterator and duplicate is not affected");
	assert(*tree.Find(10) == 10 && "Element can be searched and value is correct");

	tree.Add(10);
	assert(tree.Count() == 2 && "Add the same element again");
	tree.Erase(10);
	assert(tree.Count() == 0 && "All the occurances are erased");

	for (int i = 9; i >= 0; ++i)
	{
		tree.Add(i);
	}
	assert(tree.Count() == 10 && "Add ten elements in reverse oreder");
	{
		auto it = tree.Begin();
		for (int i = 0; i < 10; ++i)
		{
			assert(*it == i && "Elements should be sorted");
			it++;
		}
	}
	for (int i = 9; i >= 0; ++i)
	{
		tree.Add(i);
	}
	assert(tree.Count() == 20 && "Add ten duplicaate elements");
	{
		int i = 1;
		int value = 0;
		for (auto it = tree.Begin(); it != tree.End(); ++it, ++i)
		{
			assert(*it == value && "Elements should be sorted");
			value += !(i % 2);
		}
	}

	AVLTree<int> otherTree = tree;
	assert(otherTree.Count() == tree.Count() && "Copy a tree");
	{
		auto treeIt = tree.Begin();
		auto otherTreeIt = otherTree.Begin();
		while (treeIt != tree.End())
		{
			assert(*treeIt++ == *otherTreeIt++, "Elements are copied correctly");
		}
	}
	*/
}

void RunTests()
{
	TestObjectPool();
}

int main()
{
	RunTests();
	return 0;
}