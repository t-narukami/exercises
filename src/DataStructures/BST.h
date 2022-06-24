#pragma once
#include <memory>
#include "Vector.h"

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
	template <typename S>
	struct IteratorImpl;

	using ConstIterator = IteratorImpl<const T>;
	using Iterator = IteratorImpl<T>;

	ConstIterator Find(T const& v) const;
	Iterator Find(T const& v);

	void Erase(T const &v);
	void Erase(Iterator const& it);

	ConstIterator Begin() const { return { this, Min(m_root.get()) }; }
	ConstIterator End() const { return { nullptr, nullptr }; }

	Iterator Begin() { return { this, Min(m_root.get()) }; }
	Iterator End() { return { this, nullptr }; }

private:
	struct Node;
	using NodePtr = std::unique_ptr<Node>;

	struct Node
	{
		Node* parent = nullptr;
		NodePtr left = nullptr;
		NodePtr right = nullptr;

		T value;

		Node(T const& v)
			: value(v)
		{
		}
		Node(T&& v)
			: value(std::move(v))
		{
		}
		Node() = delete;
	};

	void InsertNode(Node&& node);
	Node* Min(Node* const ptr);
	Node* Max(Node* const ptr);
	NodePtr* Max(NodePtr& ptr);
	Node* Successor(Node* const ptr);
	Node* Predecessor(Node* const ptr);

	NodePtr m_root = nullptr;
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
	if (m_root)
	{
		res.m_root = std::make_unique<Node>(m_root->value);

		Node* copy = res.m_root.get();
		Node const* orig = m_root.get();
		while (copy)
		{
			if (orig->left && !copy->left)
			{
				copy->left = std::make_unique<Node>(orig->left->value);
				copy->left->parent = copy;
				copy = copy->left.get();
				orig = orig->left.get();
			}
			else if (orig->right && !copy->right)
			{
				copy->right = std::make_unique<Node>(orig->right->value);
				copy->right->parent = copy;
				copy = copy->right.get();
				orig = orig->right.get();
			}
			else
			{
				copy = copy->parent;
				orig = orig->parent;
			}
		}
	}
	return res;
}

template <typename T>
BST<T>::BST(BST const& rhs)
{
	this->swap(rhs.copy());
}

template <typename T>
BST<T>::BST(BST&& rhs) noexcept
{
	this->swap(std::move(rhs));
}

template <typename T>
BST<T>& BST<T>::operator=(BST rhs)
{
	this->swap(std::move(rhs));
	return *this;
}

template <typename T>
BST<T>& BST<T>::operator=(BST&& rhs) noexcept
{
	this->swap(std::move(rhs));
	return *this;
}

template <typename T>
void BST<T>::Add(T const& value)
{
	InsertNode({ value });
}

template <typename T>
void BST<T>::Add(T&& value)
{
	InsertNode({ std::move(value) });
}

template <typename T>
template <typename ...Args>
void BST<T>::Emplace(Args&& ...args)
{
	InsertNode({ {std::forward<Args>(args)...} });
}

template <typename T>
void BST<T>::InsertNode(Node&& node)
{
	Node* parent = nullptr;
	NodePtr* it = &m_root;
	while (*it)
	{
		parent = it->get();
		it = node.value < (*it)->value ? &(*it)->left : &(*it)->right;
	}
	*it = std::make_unique<Node>(std::move(node));
	(*it)->parent = parent;
	++m_count;
}

template <typename T>
typename BST<T>::Node* BST<T>::Min(BST<T>::Node* const ptr)
{
	Node* parent = ptr;
	Node* leaf = ptr->left.get();
	while (leaf)
	{
		parent = leaf;
		leaf = parent->left.get();
	}
	return parent;
}

template <typename T>
typename BST<T>::Node* BST<T>::Max(BST<T>::Node* const ptr)
{
	Node* parent = ptr;
	Node* leaf = ptr->right.get();
	while (leaf)
	{
		parent = leaf;
		leaf = parent->right.get();
	}
	return parent;
}

template <typename T>
typename BST<T>::NodePtr* BST<T>::Max(BST<T>::NodePtr& ptr)
{
	NodePtr* parent = &ptr;
	NodePtr* leaf = &ptr->right;
	while (*leaf)
	{
		parent = leaf;
		leaf = &(*parent)->right;
	}
	return parent;
}

template <typename T>
typename BST<T>::Node* BST<T>::Successor(BST<T>::Node* const ptr)
{
	if (ptr->right)
	{
		return Min(ptr->right.get());
	}
	Node* child = ptr;
	Node* parent = ptr->parent;
	while (parent && parent->right.get() == child)
	{
		child = parent;
		parent = child->parent;
	}
	return parent;
}

template <typename T>
typename BST<T>::Node* BST<T>::Predecessor(BST<T>::Node* const ptr)
{
	if (ptr->left)
	{
		return Max(ptr->left.get());
	}
	Node* child = ptr;
	Node* parent = ptr->parent;
	while (parent && parent->left.get() == child)
	{
		child = parent;
		parent = child->parent;
	}
	return parent;
}

template <typename T>
template <typename S>
struct BST<T>::IteratorImpl
{
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = S;
	using pointer = S*;
	using reference = S&;

	IteratorImpl() = default;
	IteratorImpl(IteratorImpl const&) = default;
	IteratorImpl& operator=(IteratorImpl const&) = default;

	operator bool() const { return m_ptr != nullptr; }

	bool operator==(IteratorImpl const& rhs) const { return m_bst == rhs.m_bst && m_ptr == rhs.m_ptr; }
	bool operator!=(IteratorImpl const& rhs) const { return !(*this == rhs); }

	pointer operator->() { return &m_ptr->value; }

	reference operator*() { return m_ptr->value; }

	IteratorImpl& operator++() { m_ptr = m_bst->Successor(m_ptr); return *this; }
	IteratorImpl& operator--() { m_ptr = m_bst->Predecessor(m_ptr); return *this; }

	IteratorImpl operator++(int)
	{
		IteratorImpl const old = *this; 
		m_ptr = m_bst->Successor(m_ptr);
		return old;
	}
	IteratorImpl operator--(int)
	{
		IteratorImpl const old = *this;
		m_ptr = m_bst->Predecessor(m_ptr);
		return old;
	}

private:
	friend class BST;
	IteratorImpl(BST<T>* bst, Node* ptr)
		: m_bst(bst)
		, m_ptr(ptr)
	{
	}

	BST<T>* m_bst = nullptr;
	Node* m_ptr = nullptr;
};

template <typename T>
typename BST<T>::ConstIterator BST<T>::Find(T const& v) const
{
	Node const* it = m_root.get();
	while (it)
	{
		if (it->value == v)
		{
			break;
		}
		it = it->value < v ? it->left.get() : it->right.get();
	}
	return { this, it };
}

template <typename T>
typename BST<T>::Iterator BST<T>::Find(T const& v)
{
	Node* it = m_root.get();
	while (it)
	{
		if (it->value == v)
		{
			break;
		}
		it = it->value < v ? it->left.get() : it->right.get();
	}
	return { this, it };
}

template <typename T>
void BST<T>::Erase(BST<T>::Iterator const& it)
{
	MY_ASSERT(it, "Erasing invalid iterator");

	Node* ptr = it.m_ptr;
	NodePtr* leaf = ptr->parent ? ptr->parent->left.get() == ptr ? &ptr->parent->left : &ptr->parent->right : &m_root;
	if (ptr->left && ptr->right)
	{
		NodePtr* predPtr = Max(ptr->left);
		Node* pred = predPtr->get();
		if (pred->parent->left.get() == pred)
		{
			pred->parent->left = nullptr;
		}
		else if (pred->parent->right.get() == pred)
		{
			pred->parent->right = nullptr;
		}
		else
		{
			MY_ASSERT("Child detached from its parent");
		}
		if (ptr->left.get() != pred)
		{
			pred->left = std::move(ptr->left);
		}
		pred->right = std::move(ptr->right);
		pred->parent = ptr->parent;
		*leaf = std::move(*predPtr);
	}
	else if (ptr->left)
	{
		ptr->left->parent = ptr->parent;
		*leaf = std::move(ptr->left);
	}
	else if (ptr->right)
	{
		ptr->right->parent = ptr->parent;
		*leaf = std::move(ptr->right);
	}
	else
	{
		*leaf = nullptr;
	}
	--m_count;
}

template <typename T>
void BST<T>::Erase(T const& v)
{
	while (Iterator it = Find(v))
	{
		Erase(it);
	}
}
