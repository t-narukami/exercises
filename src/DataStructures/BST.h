#pragma once
#include <memory>
#include "TreesCommon.h"

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

	BinaryNodeIterator<T> Find(T const& v);

	void Erase(T const& v);
	void Erase(BinaryNodeIterator<T> const& it);

	BinaryNodeIterator<T> begin() const { return { LeftMostLeaf(m_root.get()) }; }
	BinaryNodeIterator<T> end() const { return { nullptr }; }

	BinaryNodeIterator<T> begin() { return { LeftMostLeaf(m_root.get()) }; }
	BinaryNodeIterator<T> end() { return { nullptr }; }

private:
	void InsertNode(T&& value);

	BinaryNodePtr<T> m_root;
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
	res.m_root = Clone(m_root);
	return res;
}

template <typename T>
BST<T>::BST(BST const& rhs)
{
	swap(rhs.copy());
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
	BinaryNode<T>* parent = nullptr;
	BinaryNodePtr<T>* it = &m_root;
	while (*it)
	{
		parent = it->get();
		it = value < (*it)->key ? &(*it)->left : &(*it)->right;
	}
	*it = MakeBinaryNode<T>(std::move(value));
	(*it)->parent = parent;
	++m_count;
}

template <typename T>
BinaryNodeIterator<T> BST<T>::Find(T const& v)
{
	BinaryNode<T>* it = m_root.get();
	while (it)
	{
		if (it->key == v)
		{
			break;
		}
		it = v < it->key ? it->left.get() : it->right.get();
	}
	return { it };
}

template <typename T>
void BST<T>::Erase(BinaryNodeIterator<T> const& it)
{
	MY_ASSERT(it, "Erasing invalid iterator");

	BinaryNodePtr<T>* ptr = GetParentLeaf(it.GetPtr());
	if (!ptr)
	{
		ptr = &m_root;
	}
	BinaryNodePtr<T>* left = &(*ptr)->left;
	BinaryNodePtr<T>* right = &(*ptr)->right;

	--m_count;
	if (*left && *right)
	{
		// Find and detach predecessor
		BinaryNodePtr<T> predecessor = std::move(*RightMostLeaf(left));
		// Move leaves from ptr to predecessor
		if (predecessor->right = std::move(*right))
		{
			predecessor->right->parent = predecessor.get();
		}
		if (predecessor->left = std::move(*left))
		{
			predecessor->left->parent = predecessor.get();
		}
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
}

template <typename T>
void BST<T>::Erase(T const& v)
{
	while (auto it = Find(v))
	{
		Erase(it);
	}
}

