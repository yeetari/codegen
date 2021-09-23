#pragma once

#include <codegen/graph/Graph.hh>

#include <unordered_map>
#include <vector>

template <typename N>
class DepthFirstSearch {
    friend Graph<N>;

private:
    enum class State {
        Unexplored,
        Exploring,
        Explored,
    };

    std::vector<N *> m_pre_order;
    std::vector<N *> m_post_order;

    void dfs(const Graph<N> *graph, std::unordered_map<N *, State> &state, N *node);

protected:
    using Result = DepthFirstSearch<N>;
    Result run(const Graph<N> *graph);

public:
    const std::vector<N *> &pre_order() const { return m_pre_order; }
    const std::vector<N *> &post_order() const { return m_post_order; }
};

template <typename N>
void DepthFirstSearch<N>::dfs(const Graph<N> *graph, std::unordered_map<N *, State> &state, N *node) {
    m_pre_order.push_back(node);
    for (auto *succ : graph->succs(node)) {
        if (state[succ] != State::Unexplored) {
            continue;
        }
        state[succ] = State::Exploring;
        dfs(graph, state, succ);
    }
    state[node] = State::Explored;
    m_post_order.push_back(node);
}

template <typename N>
typename DepthFirstSearch<N>::Result DepthFirstSearch<N>::run(const Graph<N> *graph) {
    std::unordered_map<N *, State> state;
    state.emplace(graph->entry(), State::Exploring);
    dfs(graph, state, graph->entry());
    return std::move(*this);
}
