#pragma once

#include <codegen/support/ListNode.hh>

#include <concepts>
#include <memory>
#include <utility>

template <typename T>
class ListIterator;

template <std::derived_from<ListNode> T>
class List {
    std::unique_ptr<ListNode> m_end;

public:
    using iterator = ListIterator<T>;

    List();
    List(const List &) = delete;
    List(List &&) noexcept = default;
    ~List();

    List &operator=(const List &) = delete;
    List &operator=(List &&) noexcept = default;

    template <std::derived_from<T> U, typename... Args>
    U *emplace(iterator it, Args &&... args);
    void insert(iterator it, T *elem);
    iterator erase(iterator it);

    iterator begin() const;
    iterator end() const;

    bool empty() const;
    std::size_t size() const;
};

template <typename T>
class ListIterator {
    ListNode *m_elem;

public:
    explicit ListIterator(ListNode *elem) : m_elem(elem) {}

    ListIterator &operator++() {
        m_elem = m_elem->next();
        return *this;
    }
    ListIterator &operator--() {
        m_elem = m_elem->prev();
        return *this;
    }

    std::strong_ordering operator<=>(const ListIterator &) const = default;
    T *operator*() const { return static_cast<T *>(m_elem); }
    T *operator->() const { return static_cast<T *>(m_elem); }
    ListNode *elem() const { return m_elem; }
};

template <std::derived_from<ListNode> T>
List<T>::List() {
    m_end = std::make_unique<ListNode>();
    m_end->m_prev = m_end.get();
    m_end->m_next = m_end.get();
}

template <std::derived_from<ListNode> T>
List<T>::~List() {
    if (!m_end) {
        return;
    }
    for (auto it = begin(); it.elem() != m_end.get();) {
        auto *elem = *it;
        ++it;
        delete elem;
    }
}

template <std::derived_from<ListNode> T>
template <std::derived_from<T> U, typename... Args>
U *List<T>::emplace(iterator it, Args &&... args) {
    auto *elem = new U(std::forward<Args>(args)...);
    insert(it, elem);
    return elem;
}

template <std::derived_from<ListNode> T>
void List<T>::insert(iterator it, T *elem) {
    auto *prev = it.elem()->prev();
    elem->m_prev = prev;
    elem->m_next = it.elem();
    it.elem()->m_prev = elem;
    prev->m_next = elem;
}

template <std::derived_from<ListNode> T>
ListIterator<T> List<T>::erase(iterator it) {
    auto *prev = it->prev();
    auto *next = it->next();
    next->m_prev = prev;
    prev->m_next = next;

    auto ret = it++;
    delete *it;
    return ret;
}

template <std::derived_from<ListNode> T>
ListIterator<T> List<T>::begin() const {
    return ++end();
}

template <std::derived_from<ListNode> T>
ListIterator<T> List<T>::end() const {
    return ListIterator<T>(m_end.get());
}

template <std::derived_from<ListNode> T>
bool List<T>::empty() const {
    return size() == 0;
}

template <std::derived_from<ListNode> T>
std::size_t List<T>::size() const {
    return std::distance(begin(), end());
}

namespace std {

template <typename T>
struct iterator_traits<ListIterator<T>> {
    using difference_type = int;
    using iterator_category = std::forward_iterator_tag;
};

} // namespace std
