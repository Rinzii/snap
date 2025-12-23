#pragma once

// Must be included first
#include "snap/internal/abi_namespace.hpp"

#include <cassert>

SNAP_BEGIN_NAMESPACE

template <class Derived> struct intrusive_node_base
{
	Derived* next = nullptr;
	Derived* prev = nullptr;
};

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

	void push_front(Node* node) noexcept
	{
		node->prev = nullptr;
		node->next = m_head;

		if (m_head) { m_head->prev = node; }
		m_head = node;
	}

	Node* pop_front() noexcept
	{
		Node* front = m_head;
		m_head		= m_head->next;

		if (m_head) { m_head->prev = nullptr; }

		front->next = nullptr;
		front->prev = nullptr;

		return front;
	}

	void remove(Node* node) noexcept
	{
		if (node->prev)
		{
			node->prev->next = node->next;
			if (node->next) { node->next->prev = node->prev; }
		}
		else
		{
			assert(node == m_head && "Node to be removed has no previous link so it must be the head");
			(void)pop_front();
			return;
		}

		node->next = nullptr;
		node->prev = nullptr;
	}

	bool is_head(const Node* node) const noexcept { return node == m_head; }

private:
	Node* m_head = nullptr;
};

SNAP_END_NAMESPACE
