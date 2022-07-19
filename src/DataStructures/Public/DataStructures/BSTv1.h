#pragma once
#include "TreesCommon.h"
#include "Memory/Memory.h"
#include "Memory/SharedHandle.h"

template <typename T>
class BSTv1
{
public:
	BSTv1() = default;
	~BSTv1() = default;

	BSTv1(BSTv1 const& rhs);
	BSTv1(BSTv1&& rhs) noexcept;

	BSTv1& operator=(BSTv1 rhs);
	BSTv1& operator=(BSTv1&& rhs) noexcept;

	BSTv1 copy() const noexcept;
	void swap(BSTv1&& rhs) noexcept;

	template <typename ...Args>
	void Emplace(Args&& ...args);

	void Add(T const& v);

	void Add(T&& v);

	int Count() const { return m_count; }

public:
	struct Node;
	using NodeHandle = Memory::SharedHandle<Node>;

	using Iterator = BinaryNodes::NodeIterator<NodeHandle, T>;
	using ConstIterator = BinaryNodes::NodeIterator<NodeHandle, const T>;

	Iterator Find(T const& v) { return BinaryNodes::BinarySearch(m_root, v); }
	ConstIterator Find(T const& v) const { return BinaryNodes::BinarySearch(m_root, v); }

	void Erase(T const& v);
	void Erase(Iterator const& it);

	ConstIterator begin() const { return { BinaryNodes::LeftMostLeaf(m_root) }; }
	ConstIterator end() const { return { {} }; }

	Iterator begin() { return { BinaryNodes::LeftMostLeaf(m_root) }; }
	Iterator end() { return { {} }; }

private:
	void InsertNode(T&& value);

	struct Node
	{
		NodeHandle parent;
		NodeHandle left;
		NodeHandle right;
		T value;

		Node(T const& v) : value(v) {}
		Node(T&& v) : value(std::move(v)) {}

		NodeHandle clone() const 
		{
			return Memory::MakeShared<Node>(value);
		}

		T* operator->() { return &value; }
		T& operator*() { return value; }
		T const* operator->() const { return &value; }
		T const& operator*() const { return value; }
	};

	NodeHandle m_root;
	int m_count = 0;
};

template <typename T>
void BSTv1<T>::swap(BSTv1<T>&& rhs) noexcept
{
	std::swap(m_root, rhs.m_root);
	std::swap(m_count, rhs.m_count);
}

template <typename T>
BSTv1<T> BSTv1<T>::copy() const noexcept
{
	BSTv1<T> res;
	res.m_count = m_count;
	res.m_root = BinaryNodes::CloneTree(m_root);
	return res;
}

template <typename T>
BSTv1<T>::BSTv1(BSTv1 const& rhs)
{
	swap(rhs.copy());
}

template <typename T>
BSTv1<T>::BSTv1(BSTv1&& rhs) noexcept
{
	swap(std::move(rhs));
}

template <typename T>
BSTv1<T>& BSTv1<T>::operator=(BSTv1 rhs)
{
	swap(std::move(rhs));
	return *this;
}

template <typename T>
BSTv1<T>& BSTv1<T>::operator=(BSTv1&& rhs) noexcept
{
	swap(std::move(rhs));
	return *this;
}

template <typename T>
void BSTv1<T>::Add(T const& value)
{
	T copy = value;
	InsertNode(std::move(copy));
}

template <typename T>
void BSTv1<T>::Add(T&& value)
{
	InsertNode(std::move(value));
}

template <typename T>
template <typename ...Args>
void BSTv1<T>::Emplace(Args&& ...args)
{
	InsertNode({ std::forward<Args>(args)... });
}

template <typename T>
void BSTv1<T>::InsertNode(T&& value)
{
	NodeHandle* parent = nullptr;
	NodeHandle* it = &m_root;
	while (*it)
	{
		parent = it;
		if (value < ***it)
		{
			it = &(*it)->left;
		}
		else
		{
			it = &(*it)->right;
		}
	}
	*it = Memory::MakeShared<Node>(std::move(value));
	if (parent)
	{
		(*it)->parent = *parent;
	}
	++m_count;
}

template <typename T>
void BSTv1<T>::Erase(Iterator const& it)
{
	using namespace Nodes;
	MY_ASSERT(it, "Erasing invalid iterator");

	Node& node = *it.GetPtr();
	NodeHandle* nodePtr = node.parent ? node.parent->left == it.GetPtr() ? &node.parent->left : &node.parent->right : &m_root;
	NodeHandle* left = &(*nodePtr)->left;
	NodeHandle* right = &(*nodePtr)->right;

	if (*left && *right)
	{
		// Find and detach predecessor
		NodeHandle predecessor = std::move(BinaryNodes::RightMostLeaf(*left));
		// Move leaves from ptr to predecessor
		if (predecessor->right = std::move(*right))
		{
			predecessor->right->parent = predecessor;
		}
		if (predecessor->left = std::move(*left))
		{
			predecessor->left->parent = predecessor;
		}
		// Attach predecessor to ptr's parrent
		predecessor->parent = (*nodePtr)->parent;
		*nodePtr = std::move(predecessor);
	}
	else if (*left)
	{
		(*left)->parent = (*nodePtr)->parent;
		*nodePtr = std::move(*left);
	}
	else if (*right)
	{
		(*right)->parent = (*nodePtr)->parent;
		*nodePtr = std::move(*right);
	}
	else
	{
		*nodePtr = {};
	}
	--m_count;
}

template <typename T>
void BSTv1<T>::Erase(T const& v)
{
	while (auto it = Find(v))
	{
		Erase(it);
	}
}

