#pragma once

#include <concepts>

namespace coel {

class ListNode {
    template <std::derived_from<ListNode> T>
    friend class List;

private:
    mutable const ListNode *m_prev{nullptr};
    mutable const ListNode *m_next{nullptr};

public:
    ListNode() = default;
    ListNode(const ListNode &) = delete;
    ListNode(ListNode &&) = delete;
    virtual ~ListNode() = default;

    ListNode &operator=(const ListNode &) = delete;
    ListNode &operator=(ListNode &&) = delete;

    ListNode *prev() const { return const_cast<ListNode *>(m_prev); }
    ListNode *next() const { return const_cast<ListNode *>(m_next); }
};

} // namespace coel
