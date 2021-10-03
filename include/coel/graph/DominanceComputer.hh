#pragma once

#include <coel/graph/DepthFirstSearch.hh>
#include <coel/graph/DominatorTree.hh>
#include <coel/graph/Graph.hh>
#include <coel/support/Assert.hh>

#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <utility>

namespace coel {

template <typename N>
class DominanceComputer {
    friend Graph<N>;

protected:
    using Result = DominatorTree<N>;
    Result run(const Graph<N> *graph);
};

template <typename N>
typename DominanceComputer<N>::Result DominanceComputer<N>::run(const Graph<N> *graph) {
    // TODO: Avoid copying post order vector.
    auto dfs = graph->template run<DepthFirstSearch>();
    auto order = dfs.post_order();

    // Build map of nodes to indices in post-order.
    std::unordered_map<N *, std::size_t> index_map;
    for (auto *node : order) {
        index_map.emplace(node, index_map.size());
    }

    // Transform order into reverse post-order.
    std::reverse(order.begin(), order.end());

    std::unordered_map<N *, N *> doms;
    doms.emplace(graph->entry(), graph->entry());
    auto intersect = [&](N *finger1, N *finger2) {
        while (index_map.at(finger1) != index_map.at(finger2)) {
            while (index_map.at(finger1) < index_map.at(finger2)) {
                finger1 = doms.at(finger1);
            }
            while (index_map.at(finger2) < index_map.at(finger1)) {
                finger2 = doms.at(finger2);
            }
        }
        return finger1;
    };

    // Remove graph entry from order.
    COEL_ASSERT(order.front() == graph->entry());
    order.erase(order.begin());

    // TODO: Clean this up.
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto *b : order) {
            N *new_idom = nullptr;
            for (auto *pred : graph->preds(b)) {
                if (!graph->preds(pred).empty() || pred == graph->entry()) {
                    new_idom = pred;
                    break;
                }
            }
            for (auto *p : graph->preds(b)) {
                if (p != new_idom && doms.contains(p)) {
                    new_idom = intersect(p, new_idom);
                }
            }
            changed = doms[b] == new_idom;
            doms[b] = new_idom;
        }
    }

    // Build the dominator tree.
    DominatorTree<N> tree(graph->entry());
    for (auto [node, idom] : doms) {
        // Ignore self-domination.
        if (node != idom) {
            tree.connect(idom, node);
        }
    }
    return std::move(tree);
}

} // namespace coel
