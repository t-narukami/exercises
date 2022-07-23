#pragma once
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

public:
	struct Node;

	using Iterator = BinaryNodes::NodeIterator<Node*, T>;
	using ConstIterator = BinaryNodes::NodeIterator<Node*, const T>;

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
		Node* parent = nullptr;
		Node* left = nullptr;
		Node* right = nullptr;

		T value;

		Node() = delete;
		Node(T const& v) : value(v) {}
		Node(T&& v) : value(std::move(v)) {}

		Node* clone() const { return new Node(value); }

		T const* operator->() const { return &value; }
		T const& operator*() const { return value; }
		T* operator->() { return &value; }
		T& operator*() { return value; }

		~Node()
		{
			delete left;
			delete right;
		}
	};

	Node* m_root = nullptr;
	uint32_t m_count = 0;
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
	Node* parent = nullptr;
	Node* it = m_root;
	bool left = false;
	while (it)
	{
		parent = it;
		if (value < it->value)
		{
			it = it->left;
			left = true;
		}
		else
		{
			it = it->right;
			left = false;
		}
	}
	Node* nodePtr = new Node(std::move(value));
	if (parent == nullptr)
	{
		m_root = nodePtr;
	}
	else if (left)
	{
		parent->left = nodePtr;
		parent->left->parent = parent;
	}
	else
	{
		parent->right = nodePtr;
		parent->right->parent = parent;
	}
	++m_count;
}

template <typename T>
void BST<T>::Erase(Iterator const& it)
{
	MY_ASSERT(it, "Erasing invalid iterator");

	Node* ptr = it.GetPtr();

	auto const ReplaceNode = [this](Node* old, Node* replacement) {
		if (replacement)
		{
			replacement->parent = old->parent;
		}
		if (old->parent)
		{
			if (old->parent->left == old)
			{
				old->parent->left = replacement;
			}
			else
			{
				old->parent->right = replacement;
			}
		}
		else
		{
			m_root = replacement;
		}
	};

	if (ptr->left && ptr->right)
	{
		Node* predecessor = BinaryNodes::RightMostLeaf(ptr->left);
		// Detach predecessor
		ReplaceNode(predecessor, nullptr);

		// Reattach leaves to predecessor
		predecessor->right = ptr->right;
		predecessor->left = ptr->left;
		ptr->right->parent = predecessor;
		ptr->left->parent = predecessor;

		// Attach predecessor in place of ptr
		ReplaceNode(ptr, predecessor);
	}
	else if (ptr->left)
	{
		ReplaceNode(ptr, ptr->left);
	}
	else if (ptr->right)
	{
		ReplaceNode(ptr, ptr->right);
	}
	else
	{
		ReplaceNode(ptr, nullptr);
	}

	ptr->right = nullptr;
	ptr->left = nullptr;
	delete ptr;
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

