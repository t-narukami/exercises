#pragma once
#include "TreesCommon.h"
#include "MemPool.h"

template <typename T, typename Allocator>
class BST
{
public:
	BST(Allocator& allocator) : m_allocator(allocator) {}
	~BST() = default;

	BST(BST const& rhs);
	BST(BST&& rhs) noexcept;

	BST& operator=(BST rhs);
	BST& operator=(BST&& rhs) noexcept;

	BST copy() const noexcept;
	void swap(BST&& rhs) noexcept;

	template <typename ...Args>
	void Emplace(Args&& ...args);

	void Add(T const& v);

	void Add(T&& v);

	int Count() const { return m_count; }

public:
	struct Node;
	using NodeHandle = Allocator::Handle<Node>;

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

		NodeHandle clone() const { return m_allocator.Make<Node>(*this); } // TODO

		T* operator->() { return &value; }
		T& operator*() { return value; }
		T const* operator->() const { return &value; }
		T const& operator*() const { return value; }
	};

	NodeHandle m_root;
	int m_count = 0;
	Allocator& m_allocator;
};

template <typename T, typename Allocator>
void BST<T, Allocator>::swap(BST<T, Allocator>&& rhs) noexcept
{
	std::swap(m_root, rhs.m_root);
	std::swap(m_count, rhs.m_count);
	m_allocator = rhs.m_allocator;
}

template <typename T, typename Allocator>
BST<T, Allocator> BST<T, Allocator>::copy() const noexcept
{
	BST<T, Allocator> res;
	res.m_count = m_count;
	res.m_root = BinaryNodes::CloneTree(m_root);
	res.m_allocator = m_allocator;
	return res;
}

template <typename T, typename Allocator>
BST<T, Allocator>::BST(BST const& rhs)
{
	swap(rhs.copy());
}

template <typename T, typename Allocator>
BST<T, Allocator>::BST(BST&& rhs) noexcept
{
	swap(std::move(rhs));
}

template <typename T, typename Allocator>
BST<T, Allocator>& BST<T, Allocator>::operator=(BST rhs)
{
	swap(std::move(rhs));
	return *this;
}

template <typename T, typename Allocator>
BST<T, Allocator>& BST<T, Allocator>::operator=(BST&& rhs) noexcept
{
	swap(std::move(rhs));
	return *this;
}

template <typename T, typename Allocator>
void BST<T, Allocator>::Add(T const& value)
{
	T copy = value;
	InsertNode(std::move(copy));
}

template <typename T, typename Allocator>
void BST<T, Allocator>::Add(T&& value)
{
	InsertNode(std::move(value));
}

template <typename T, typename Allocator>
template <typename ...Args>
void BST<T, Allocator>::Emplace(Args&& ...args)
{
	InsertNode({ std::forward<Args>(args)... });
}

template <typename T, typename Allocator>
void BST<T, Allocator>::InsertNode(T&& value)
{
	NodeHandle parent;
	NodeHandle* it = &m_root;
	while (*it)
	{
		parent = *it;
		it = value < *it ? &(*it)->left : &(*it)->right;
	}
	*it = m_allocator.Make<Node>(std::move(value)); // TODO
	(*it)->parent = parent;
	++m_count;
}

template <typename T, typename Allocator>
void BST<T, Allocator>::Erase(Iterator const& it)
{
	using namespace Nodes;
	MY_ASSERT(it, "Erasing invalid iterator");

	/*
	BinaryNodeHandle<T>* ptr = GetLeafHandle(it.GetPtr());
	if (!ptr)
	{
		ptr = &m_root;
	}
	BinaryNodeHandle<T>* left = &(*ptr)->left;
	BinaryNodeHandle<T>* right = &(*ptr)->right;

	--m_count;
	if (*left && *right)
	{
		// Find and detach predecessor
		BinaryNodeHandle<T> predecessor = std::move(*RightMostLeaf(left));
		// Move leaves from ptr to predecessor
		SetRightLeaf(predecessor.get(), std::move(*right));
		SetLeftLeaf(predecessor.get(), std::move(*left));
		// Attach predecessor to ptr's parrent
		predecessor->parent = (*ptr)->parent;
		*ptr = std::move(predecessor);
	}
	else if (*left)
	{
		(*left)->parent = (*ptr)->parent;
		*ptr = std::move(*left);
	}
	else if (*right)
	{
		(*right)->parent = (*ptr)->parent;
		*ptr = std::move(*right);
	}
	else
	{
		BinaryNode<T>* parent = (*ptr)->parent;
		*ptr = nullptr;
	}
	*/
}

template <typename T, typename Allocator>
void BST<T, Allocator>::Erase(T const& v)
{
	while (auto it = Find(v))
	{
		Erase(it);
	}
}

