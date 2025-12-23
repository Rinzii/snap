#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cassert>

SNAP_BEGIN_NAMESPACE
// A tiny base you can inherit from to get next/prev links.
// Example: struct Node : snap::intrusive_node_base<Node> { /* ... */ };
template <class Derived> struct intrusive_node_base
{
	Derived* next = nullptr;
	Derived* prev = nullptr;
};

// Non-owning doubly-linked list view over nodes that provide next/prev links.
// This does NOT manage node lifetimes; it only links/unlinks them.
template <class Node> class intrusive_list_view
{
public:
	intrusive_list_view()									   = default;
	intrusive_list_view(const intrusive_list_view&)			   = default;
	intrusive_list_view(intrusive_list_view&&)				   = default;
	intrusive_list_view& operator=(const intrusive_list_view&) = default;
	intrusive_list_view& operator=(intrusive_list_view&&)	   = default;
	~intrusive_list_view()									   = default;

	bool empty() const noexcept { return m_head == nullptr; }

	// Insert at front.
	void push_front(Node* node) noexcept
	{
		node->next = m_head;
		if (m_head) { m_head->prev = node; }
		m_head = node;
	}

	// Remove and return the front node. Precondition: not empty().
	Node* pop_front() noexcept
	{
		Node* front = m_head;
		m_head		= m_head->next;
		if (m_head) { m_head->prev = nullptr; }

		return front;
	}

	// Remove an arbitrary node already in the list.
	void remove(Node* node) noexcept
	{
		if (node->prev == nullptr)
		{
			node->prev->next = node->next;
			if (node->next) { node->next->prev = node->prev; }
		}
		else
		{
			assert(node == m_head && "Node to be removed has no previous link so it must be the head");
			pop_front();
		}
	}

	bool is_head(const Node* node) const noexcept { return node == m_head; }

private:
	Node* m_head = nullptr;
};

SNAP_END_NAMESPACE
