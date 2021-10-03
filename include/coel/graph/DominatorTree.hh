#pragma once

#include <coel/graph/Graph.hh>
#include <coel/support/Assert.hh>

namespace coel {

template <typename N>
struct DominatorTree : public Graph<N> {
    explicit DominatorTree(N *entry) : Graph<N>(entry) {}

    N *idom(const N *node) const;
};

template <typename N>
N *DominatorTree<N>::idom(const N *node) const {
    if (node == this->entry()) {
        return nullptr;
    }
    COEL_ASSERT(this->preds(node).size() == 1);
    return this->preds(node)[0];
}

} // namespace coel
