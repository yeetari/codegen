#pragma once

#include <unordered_map>
#include <utility>
#include <vector>

namespace coel {

template <typename N>
class Graph {
    std::unordered_map<const N *, std::vector<N *>> m_preds;
    std::unordered_map<const N *, std::vector<N *>> m_succs;
    N *m_entry;

public:
    explicit Graph(N *entry) : m_entry(entry) {}
    Graph(const Graph &) = delete;
    Graph(Graph &&other) noexcept
        : m_preds(std::move(other.m_preds)), m_succs(std::move(other.m_succs)),
          m_entry(std::exchange(other.m_entry, nullptr)) {}
    ~Graph() = default;

    Graph &operator=(const Graph &) = delete;
    Graph &operator=(Graph &&) = delete;

    void connect(N *src, N *dst);
    void disconnect(N *src, N *dst);
    template <template <typename> typename T, typename U = T<N>>
    typename U::Result run() const {
        return U().run(this);
    }
    void set_entry(N *entry) { m_entry = entry; }

    const std::vector<N *> &preds(const N *node) const;
    const std::vector<N *> &succs(const N *node) const;

    N *entry() const { return m_entry; }
};

template <typename N>
void Graph<N>::connect(N *src, N *dst) {
    m_preds[dst].push_back(src);
    m_succs[src].push_back(dst);
}

template <typename N>
void Graph<N>::disconnect(N *src, N *dst) {
    auto &pred_vec = m_preds.at(dst);
    auto &succ_vec = m_succs.at(src);
    auto pred_it = std::find(pred_vec.begin(), pred_vec.end(), src);
    auto succ_it = std::find(succ_vec.begin(), succ_vec.end(), dst);
    pred_vec.erase(pred_it);
    succ_vec.erase(succ_it);
}

template <typename N>
const std::vector<N *> &Graph<N>::preds(const N *node) const {
    return const_cast<Graph<N> *>(this)->m_preds[node];
}

template <typename N>
const std::vector<N *> &Graph<N>::succs(const N *node) const {
    return const_cast<Graph<N> *>(this)->m_succs[node];
}

} // namespace coel
