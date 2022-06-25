#include <cassert>
#include <iostream>
#include <string>

#include "Assert.h"
#include "BST.h"
#include "Testy.h"
#include "Vector.h"

void TestVector()
{
	TEST("Vector test");
	Vector<std::string> vector;
	ASSERT(vector.Count() == 0, "Construct and empty vector of strings");
	vector.Add("first");
	ASSERT(vector.Count() == 1, "Add an element");
	ASSERT(vector.At(0) == "first", "Element is correct");
	vector.Emplace("second");
	ASSERT(vector.Count() == 2, "Emplace another element");
	ASSERT(vector.At(1) == "second", "Another element is also correct");
	vector.Erase(0);
	ASSERT(vector.Count() == 1 , "Remove first element");
	ASSERT(vector.At(0) == "second", "Second element is now first");
	vector.Clear();
	ASSERT(vector.Count() == 0, "Clear the vector");

	for (int i = 0; i < 10; ++i)
	{
		vector.Add(std::to_string(i));
	}
	ASSERT(vector.Count() == 10, "Add 10 elemets");

	for (int i = 0; i < vector.Count(); ++i)
	{
		ASSERT(vector.At(i) == std::to_string(i), "All the elements are correct");
	}

	Vector<std::string> anotherVector = vector;
	ASSERT(anotherVector.Count() == vector.Count(), "Copy the vector");

	for (int i = 0; i < vector.Count(); ++i)
	{
		ASSERT(anotherVector.At(i) == vector.At(i), "Correct elements are copied");
	}

	anotherVector.Erase(anotherVector.Count() - 1);
	ASSERT(anotherVector.Count() == 9, "Erase last element");

	{
		Vector<Testy::OnlyMovable> data;
		data.Add(Testy::OnlyMovable(10));
		ASSERT(data.At(0).value == 10 , "Add with move constructor");
	}
	{
		Vector<Testy::OnlyCopyable> data;
		Testy::OnlyCopyable obj(10);
		data.Add(obj);
		ASSERT(data.At(0).value == 10, "Add with move constructor");
	}
	{
		Vector<Testy::NonTriviallyConstructable> data;
		data.Add(Testy::NonTriviallyConstructable(10));
		ASSERT(data.At(0).value == 10, "Add with move constructor");
		Testy::NonTriviallyConstructable obj(11);
		data.Add(obj);
		ASSERT(data.At(1).value == 11, "Add with copy constructor");
		data.Emplace(12);
		ASSERT(data.At(2).value == 12, "Emplace with arguments");
	}
}

template <typename T>
void TestTree()
{
	TEST("Test tree");
	T tree;
	ASSERT(tree.Count() == 0, "Construct empty tree");

	tree.Add(10);
	ASSERT(tree.Count() == 1 , "Add one element");
	ASSERT(*tree.begin() == 10 , "Element is at the start");
	ASSERT(tree.Find(10), "Element can be searched, operator bool");
	ASSERT(*tree.Find(10) == 10 , "Element can be searched and value is correct");

	tree.Add(10);
	ASSERT(tree.Count() == 2 , "Add the same element");
	ASSERT(*tree.Find(10) == 10 , "Element is searched correctly");
	tree.Erase(tree.Find(10));
	ASSERT(tree.Count() == 1 , "Element is deleted by iterator and duplicate is not affected");
	ASSERT(*tree.Find(10) == 10 , "Element can be searched and value is correct");

	tree.Add(10);
	ASSERT(tree.Count() == 2 , "Add the same element again");
	tree.Erase(10);
	ASSERT(tree.Count() == 0 , "All the occurances are erased");

	for (int i = 9; i >= 0; --i)
	{
		tree.Add(i);
	}
	ASSERT(tree.Count() == 10 , "Add ten elements in reverse oreder");
	{
		auto it = tree.begin();
		for (int i = 0; i < 10; ++i)
		{
			ASSERT(*it == i , "Elements should be sorted");
			it++;
		}
	}
	for (int i = 9; i >= 0; --i)
	{
		tree.Add(i);
	}
	ASSERT(tree.Count() == 20 , "Add ten duplicaate elements");
	{
		int i = 1;
		int value = 0;
		for (auto it = tree.begin(); it != tree.end(); ++it, ++i)
		{
			ASSERT(*it == value , "Elements should be sorted");
			value += !(i % 2);
		}
	}

	T otherTree = tree;
	ASSERT(otherTree.Count() == tree.Count() , "Copy a tree");
	{
		auto treeIt = tree.begin();
		auto otherTreeIt = otherTree.begin();
		while (treeIt != tree.end())
		{
			ASSERT(*treeIt++ == *otherTreeIt++, "Elements are copied correctly");
		}
	}
}

void RunTests()
{
	TestVector();
	TestTree<BST<int>>();
}

int main()
{
	RunTests();

	return 0;
}