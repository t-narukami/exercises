#include <iostream>
#include <cassert>
#include <string>

#include "Testy.h"
#include "ObjectPool.h"

void TestObjectPool()
{
	TEST("Object pool test");
	ObjectPool<std::string> pool;
	ASSERT(pool.Count() == 0, "Construct and empty pool of strings");
	pool.Add("first");
	ASSERT(pool.Count() == 1, "Add an element");
	ASSERT(pool.At(0) == "first", "Element is correct");
	pool.Emplace("second");
	ASSERT(pool.Count() == 2, "Emplace another element");
	ASSERT(pool.At(1) == "second", "Another element is also correct");
	pool.Erase(0);
	ASSERT(pool.Count() == 1 , "Remove first element");
	ASSERT(pool.At(0) == "second", "Second element is now first");
	pool.Clear();
	ASSERT(pool.Count() == 0, "Clear the pool");

	for (int i = 0; i < 10; ++i)
	{
		pool.Add(std::to_string(i));
	}
	ASSERT(pool.Count() == 10, "Add 10 elemets");

	for (int i = 0; i < pool.Count(); ++i)
	{
		ASSERT(pool.At(i) == std::to_string(i), "All the elements are correct");
	}

	ObjectPool<std::string> anotherPool = pool;
	ASSERT(anotherPool.Count() == pool.Count(), "Copy the pool");

	for (int i = 0; i < pool.Count(); ++i)
	{
		ASSERT(anotherPool.At(i) == pool.At(i), "Correct elements are copied");
	}

	anotherPool.Erase(anotherPool.Count() - 1);
	ASSERT(anotherPool.Count() == 9, "Erase last element");

	{
		ObjectPool<Testy::OnlyMovable> data;
		data.Add(Testy::OnlyMovable(10));
		ASSERT(data.At(0).value == 10 , "Add with move constructor");
	}
	{
		ObjectPool<Testy::OnlyCopyable> data;
		Testy::OnlyCopyable obj(10);
		data.Add(obj);
		ASSERT(data.At(0).value == 10, "Add with move constructor");
	}
	{
		ObjectPool<Testy::NonTriviallyConstructable> data;
		data.Add(Testy::NonTriviallyConstructable(10));
		ASSERT(data.At(0).value == 10, "Add with move constructor");
		Testy::NonTriviallyConstructable obj(11);
		data.Add(obj);
		ASSERT(data.At(1).value == 11, "Add with copy constructor");
		data.Emplace(12);
		ASSERT(data.At(2).value == 12, "Emplace with arguments");
	}
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