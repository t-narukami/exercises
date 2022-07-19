#pragma once
#include "TreesCommon.h"
#include <memory>

template <typename T>
class BST
{
public:
	BST() = default;
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
	using NodeHandle = std::shared_ptr<Node>;

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
			return std::make_shared<Node>(value);
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
void BST<T>::swap(BST<T>&& rhs) noexcept
{
	std::swap(m_root, rhs.m_root);
	std::swap(m_count, rhs.m_count);
}

template <typename T>
BST<T> BST<T>::copy() const noexcept
{
	BST<T> res;
	res.m_count = m_count;
	res.m_root = BinaryNodes::CloneTree(m_root);
	return res;
}

template <typename T>
BST<T>::BST(BST const& rhs)
{
	swap(std::move(rhs.copy()));
}

template <typename T>
BST<T>::BST(BST&& rhs) noexcept
{
	swap(std::move(rhs));
}

template <typename T>
BST<T>& BST<T>::operator=(BST rhs)
{
	swap(std::move(rhs));
	return *this;
}

template <typename T>
BST<T>& BST<T>::operator=(BST&& rhs) noexcept
{
	swap(std::move(rhs));
	return *this;
}

template <typename T>
void BST<T>::Add(T const& value)
{
	T copy = value;
	InsertNode(std::move(copy));
}

template <typename T>
void BST<T>::Add(T&& value)
{
	InsertNode(std::move(value));
}

template <typename T>
template <typename ...Args>
void BST<T>::Emplace(Args&& ...args)
{
	InsertNode({ std::forward<Args>(args)... });
}

template <typename T>
void BST<T>::InsertNode(T&& value)
{
	NodeHandle parent;
	NodeHandle* it = &m_root;
	while (*it)
	{
		parent = *it;
		it = value < ***it ? &(*it)->left : &(*it)->right;
	}
	*it = std::make_shared<Node>(std::move(value));
	(*it)->parent = parent;
	++m_count;
}

template <typename T>
void BST<T>::Erase(Iterator const& it)
{
	using namespace Nodes;
	MY_ASSERT(it, "Erasing invalid iterator");

	NodeHandle node = it.GetPtr();
	NodeHandle* nodePtr = node->parent ? node->parent->left == it.GetPtr() ? &node->parent->left : &node->parent->right : &m_root;
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
		*nodePtr = nullptr;
	}
	--m_count;
}

template <typename T>
void BST<T>::Erase(T const& v)
{
	while (auto it = Find(v))
	{
		Erase(it);
	}
}

