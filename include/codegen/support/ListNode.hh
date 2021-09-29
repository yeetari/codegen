#pragma once

#include <concepts>

class ListNode {
    template <std::derived_from<ListNode> T>
    friend class List;

private:
    ListNode *m_prev{nullptr};
    ListNode *m_next{nullptr};

public:
    ListNode() = default;
    ListNode(const ListNode &) = delete;
    ListNode(ListNode &&) = delete;
    virtual ~ListNode() = default;

    ListNode &operator=(const ListNode &) = delete;
    ListNode &operator=(ListNode &&) = delete;

    ListNode *prev() const { return m_prev; }
    ListNode *next() const { return m_next; }
};
