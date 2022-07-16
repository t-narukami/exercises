#include "Tests.h"
#include "Utils/Testy.h"

#include "Vector.h"
#include "BST.h"

void TestVector()
{
	TEST("Test Vector");
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


template <template <typename> class T, typename V>
void TestBST(std::string const& testName = "Test Binary Search Tree")
{
	TEST(testName);

	int const sampleCount = 100;
	int const sampleModulo = 1000;
	auto const FillTree = [&sampleCount, &sampleModulo](T<V>& tree)
	{
		for (int i = 0; i < sampleCount; ++i)
		{
			tree.Add({ static_cast<V>(rand() % sampleModulo) });
		}
	};
	auto const Equal = [](T<V> const& lhs, T<V> const& rhs)
	{
		if (lhs.Count() != rhs.Count())
		{
			return false;
		}
		auto lhsIt = lhs.begin();
		auto rhsIt = rhs.begin();
		while (lhsIt != lhs.end())
		{
			if (*lhsIt++ != *rhsIt++)
			{
				return false;
			}
		}
		return true;
	};

	{
		T<V> tree;
		ASSERT(tree.Count() == 0, "Construct an empty tree");
	}
	{
		T<V> tree;
		V const lvalue = { static_cast<V>(10) };
		tree.Add(lvalue);
		ASSERT(tree.Count() == 1, "Add an element by lvalue");
		tree.Add({ static_cast<V>(11) });
		ASSERT(tree.Count() == 2, "Add an element by rvalue");
		tree.Emplace(static_cast<V>(12));
		ASSERT(tree.Count() == 3, "Emplace an element");
	}
	{
		T<V> tree;
		V const value = { static_cast<V>(10) };
		tree.Add(value);
		ASSERT(tree.Find(value), "Element can be searched");
		ASSERT(*tree.Find(value) == value, "Search yeild correct element");
		tree.Erase(value);
		ASSERT(tree.Count() == 0, "Element can be erased");
		ASSERT(!tree.Find(value), "Can search empty tree");
	}
	{
		T<V> tree;
		V const value = { static_cast<V>(10) };
		tree.Add(value);
		tree.Add(value);
		tree.Add(value);
		ASSERT(tree.Count() == 3, "Add duplicate elements");
		tree.Erase(tree.Find(value));
		ASSERT(tree.Count() == 2, "Element erased by iterator doesn't affect duplicates");
		tree.Erase(value);
		ASSERT(tree.Count() == 0, "All occuances are erased by value");
	}
	{
		T<V> tree;
		for (int i = sampleCount; i >= 0; --i)
		{
			tree.Add({ static_cast<V>(i) });
		}

		auto it = tree.begin();
		ASSERT(it, "begin() returns valid iterator");
		ASSERT(!tree.end(), "end() returns invalid iterator");
		ASSERT(it != tree.end(), "Iterators can be compared");
	
		for (int i = 0; i < sampleCount; ++i, ++it)
		{
			ASSERT(*it == V{ static_cast<V>(i) }, "Can iterate over elements in order");
		}
		it = tree.begin();
		for (int i = 0; i < sampleCount; ++i)
		{
			ASSERT(*it++ == V{ static_cast<V>(i) }, "Can iterate over elements in order, post increment");
		}
		int i = 0;
		for (V const& value : tree)
		{
			ASSERT(value == V{ static_cast<V>(i++) }, "Can iterate over elements in for-range loop");
		}
		ASSERT(i == sampleCount + 1, "Correct number of elements are iterated");
	}
	{
		T<V> tree;
		FillTree(tree);
		{
			auto it = tree.begin();
			V previousValue = *it;
			for (; it != tree.end(); ++it)
			{
				ASSERT(previousValue <= *it, "Random elements are in order");
				previousValue = *it;
			}
		}
		for (int i = 0; i < sampleModulo / 2; ++i)
		{
			tree.Erase({ static_cast<V>(i) });
		}
		{
			auto it = tree.begin();
			V previousValue = *it;
			for (; it != tree.end(); ++it)
			{
				ASSERT(previousValue <= *it, "Random elements are in order");
				previousValue = *it;
			}
		}
		for (int i = sampleModulo / 2; i < sampleModulo; ++i)
		{
			tree.Erase({ static_cast<V>(i) });
		}
		ASSERT(tree.Count() == 0, "Correctly removed all elements");
	}
	{
		T<V> orig;
		FillTree(orig);
		T<V> copy(orig);
		ASSERT(Equal(orig, copy), "Can copy construct a tree");
	}
	{
		T<V> orig;
		FillTree(orig);
		T<V> copy = orig;
		ASSERT(Equal(orig, copy), "Can copy assign a tree");
	}
	{
		T<V> orig;
		FillTree(orig);
		T<V> copy = orig;
		T<V> movedOrig(std::move(orig));
		ASSERT(Equal(copy, movedOrig), "Can move construct a tree");
	}
	{
		T<V> orig;
		FillTree(orig);
		T<V> copy = orig;
		T<V> movedOrig = std::move(orig);
		ASSERT(Equal(copy, movedOrig), "Can move assign a tree");
	}
}

void TestDataStructures()
{
	TestVector();
	TestBST<BST, int>();
	TestBST<BST, double>();
}