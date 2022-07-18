#pragma once
#include <memory>
#include "Utils/Assert.h"

namespace BinaryNodes
{
/*
Node is assumed to have at least
struct Node
{
	NodePtr parent;
	NodePtr left;
	NodePtr right;

	Value& operator*()
	NodePtr clone() const;
};
*/

template <typename NodePtr>
NodePtr LeftMostLeaf(NodePtr root)
{
	NodePtr it = root;
	while (it && it->left)
	{
		it = it->left;
	}
	return it;
}

template <typename NodePtr>
NodePtr RightMostLeaf(NodePtr root)
{
	NodePtr it = root;
	while (it && it->right)
	{
		it = it->right;
	}
	return it;
}

template <typename NodePtr>
NodePtr Predecessor(NodePtr root)
{
	if (!root)
	{
		return root;
	}
	if (root->left)
	{
		return RightMostLeaf(root->left);
	}
	NodePtr parent = root->parent;
	NodePtr child = root;
	while (parent && parent->left == child)
	{
		child = parent;
		parent = parent->parent;
	}
	return parent;
}

template <typename NodePtr>
NodePtr Successor(NodePtr root)
{
	if (!root)
	{
		return root;
	}
	if (root->right)
	{
		return LeftMostLeaf(root->right);
	}
	NodePtr parent = root->parent;
	NodePtr child = root;
	while (parent && parent->right == child)
	{
		child = parent;
		parent = parent->parent;
	}
	return parent;
}

template <typename NodePtr>
NodePtr CloneTree(NodePtr root)
{
	if (!root)
	{
		return root;
	}
	NodePtr copyRoot = root->clone();
	NodePtr orig = root;
	NodePtr copy = copyRoot;
	while (copy)
	{
		if (orig->left && !copy->left)
		{
			copy->left = orig->left->clone();
			copy->left->parent = copy;
			copy = copy->left;
			orig = orig->left;
		}
		else if (orig->right && !copy->right)
		{
			copy->right = orig->right->clone();
			copy->right->parent = copy;
			copy = copy->right;
			orig = orig->right;
		}
		else
		{
			copy = copy->parent;
			orig = orig->parent;
		}
	}
	return copyRoot;
}

// Assumes Node has Value& operator*()
template <typename NodePtr, typename Value>
NodePtr BinarySearch(NodePtr root, Value const& v)
{
	NodePtr it = root;
	while (it && **it != v)
	{
		it = v < **it ? it->left : it->right;
	}
	return it;
}

template <typename NodePtr, typename Value>
struct NodeIterator
{
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = Value;
	using pointer = Value*;
	using reference = Value&;

	NodeIterator() = default;
	NodeIterator(NodePtr ptr) : m_ptr(ptr) {}

	operator bool() const { return !!m_ptr; }
	
	pointer operator->() { return m_ptr->operator->()(); }
	reference operator*() { return **m_ptr; }

	NodeIterator& operator++() { m_ptr = Successor(m_ptr); return *this; }
	NodeIterator& operator--() { m_ptr = Predecessor(m_ptr); return *this; }

	NodeIterator operator++(int)
	{
		NodeIterator const old = *this; 
		this->operator++();
		return old;
	}

	NodeIterator operator--(int)
	{
		NodeIterator const old = *this;
		this->operator--();
		return old;
	}

	NodePtr GetPtr() const { return m_ptr; }

private:
	NodePtr m_ptr;
};

} // namespace BinaryNodes

namespace Nodes 
{
template <typename T>
struct BinaryNode;

template <typename T>
using BinaryNodeHandle = std::unique_ptr<BinaryNode<T>>;

template <typename T>
struct BinaryNode
{
	BinaryNode* parent = nullptr;
	BinaryNodeHandle<T> left = nullptr;
	BinaryNodeHandle<T> right = nullptr;
	T key;

	BinaryNode() = delete;
	BinaryNode(T const& keyValue) : key(keyValue) {}
	BinaryNode(T&& keyValue) : key(std::move(keyValue)) {}
};

template <typename T>
BinaryNodeHandle<T> MakeBinaryNode(T const& value)
{
	return std::make_unique<BinaryNode<T>>(value);
}

template <typename T>
BinaryNodeHandle<T> MakeBinaryNode(T&& value)
{
	return std::make_unique<BinaryNode<T>>(std::move(value));
}

template <typename T>
void SetLeftLeaf(BinaryNode<T>* node, BinaryNodeHandle<T>&& leaf)
{
	if (node->left = std::move(leaf))
	{
		node->left->parent = node;
	}
}

template <typename T>
void SetRightLeaf(BinaryNode<T>* node, BinaryNodeHandle<T>&& leaf)
{
	if (node->right = std::move(leaf))
	{
		node->right->parent = node;
	}
}

template <typename T>
BinaryNodeHandle<T>* RightMostLeaf(BinaryNodeHandle<T>* const root)
{
	if (!root) 
	{
		return nullptr;
	}
	
	BinaryNodeHandle<T>* parent = root;
	BinaryNodeHandle<T>* leaf = &(*root)->right;
	while (*leaf)
	{
		parent = leaf;
		leaf = &(*parent)->right;
	}
	return parent;
}

template <typename T>
BinaryNode<T>* RightMostLeaf(BinaryNode<T>* const root)
{
	if (!root) 
	{
		return nullptr;
	}
	
	BinaryNode<T>* parent = root;
	BinaryNode<T>* leaf = root->right.get();
	while (leaf)
	{
		parent = leaf;
		leaf = parent->right.get();
	}
	return parent;
}

template <typename T>
BinaryNode<T>* LeftMostLeaf(BinaryNode<T>* const root)
{
	if (!root) 
	{
		return nullptr;
	}
	
	BinaryNode<T>* parent = root;
	BinaryNode<T>* leaf = root->left.get();
	while (leaf)
	{
		parent = leaf;
		leaf = parent->left.get();
	}
	return parent;
}

template <typename T>
BinaryNode<T>* Successor(BinaryNode<T>* const root)
{
	if (!root)
	{
		return nullptr;
	}
	if (root->right)
	{
		return LeftMostLeaf(root->right.get());
	}
	BinaryNode<T>* child = root;
	BinaryNode<T>* parent = root->parent;
	while (parent && parent->right.get() == child)
	{
		child = parent;
		parent = child->parent;
	}
	return parent;
}

template <typename T>
BinaryNode<T>* Predecessor(BinaryNode<T>* const root)
{
	if (!root)
	{
		return nullptr;
	}
	if (root->left)
	{
		return RightMostLeaf(root->left.get());
	}
	BinaryNode<T>* child = root;
	BinaryNode<T>* parent = root->parent;
	while (parent && parent->left.get() == child)
	{
		child = parent;
		parent = child->parent;
	}
	return parent;
}

template <typename T>
BinaryNodeHandle<T>* GetLeafHandle(BinaryNode<T> const* leaf)
{
	if (!leaf || !leaf->parent)
	{
		return nullptr;
	}
	if (leaf->parent->left.get() == leaf)
	{
		return &leaf->parent->left;
	}
	if (leaf->parent->right.get() == leaf)
	{
		return &leaf->parent->right;
	}
	MY_ASSERT(false, "Child is detached from its parent");
	return nullptr;
}

template <typename T>
BinaryNodeHandle<T> Clone(BinaryNodeHandle<T> const& root) noexcept
{ 
	BinaryNodeHandle<T> cloneRoot = MakeBinaryNode<T>(root->key);

	BinaryNode<T>* clone = cloneRoot.get();
	BinaryNode<T>* orig = root.get();

	while (clone)
	{
		if (orig->left && !clone->left)
		{
			SetLeftLeaf(clone, MakeBinaryNode<T>(orig->left->key));
			clone = clone->left.get();
			orig = orig->left.get();
		}
		else if (orig->right && !clone->right)
		{
			SetRightLeaf(clone, MakeBinaryNode<T>(orig->right->key));
			clone = clone->right.get();
			orig = orig->right.get();
		}
		else
		{
			clone = clone->parent;
			orig = orig->parent;
		}
	}

	return std::move(cloneRoot);
}

template <typename T>
struct BinaryNodeIterator
{
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = T;
	using pointer = T*;
	using reference = T&;

	BinaryNodeIterator() = default;
	BinaryNodeIterator(BinaryNodeIterator const&) = default;
	BinaryNodeIterator(BinaryNodeIterator&&) = default;
	BinaryNodeIterator& operator=(BinaryNodeIterator const&) = default;
	BinaryNodeIterator& operator=(BinaryNodeIterator&&) = default;

	BinaryNodeIterator(BinaryNode<T>* ptr) : m_ptr(ptr) {}

	operator bool() const { return m_ptr != nullptr; }
	
	pointer operator->() { return &m_ptr->key; }
	reference operator*() { return m_ptr->key; }

	const pointer operator->() const { return &m_ptr->key; }
	const reference operator*() const { return m_ptr->key; }

	BinaryNodeIterator& operator++() { m_ptr = Successor(m_ptr); return *this; }
	BinaryNodeIterator& operator--() { m_ptr = Predecessor(m_ptr); return *this; }

	BinaryNodeIterator operator++(int)
	{
		BinaryNodeIterator const old = *this; 
		m_ptr = Successor(m_ptr);
		return old;
	}

	BinaryNodeIterator operator--(int)
	{
		BinaryNodeIterator const old = *this;
		m_ptr = Predecessor(m_ptr);
		return old;
	}

	BinaryNode<T> const* GetPtr() const { return m_ptr; }

private:
	BinaryNode<T>* m_ptr = nullptr;
};

template <typename T>
BinaryNodeIterator<T> BinarySearch(BinaryNode<T> const* const root, T const& v)
{
	BinaryNode<T>* it = const_cast<BinaryNode<T>*>(root);
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
} // namespace Nodes
