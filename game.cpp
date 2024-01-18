#include "game.h"
#include <algorithm>

Game::Game(int n, int start) : g(n), player(n), component(n), component_graph(n), start(start) {
}

void Game::add_edge(int from, int to) {
    g[from].push_back(to);
}

void Game::fill_components() {
    // Sorry guys, but I cannot do it another way
    vector<vector<int>>& graph = g;
    int n = graph.size();
    vector<vector<int>> reversed_graph(graph.size());
    for (int v = 0; v < n; ++v) {
        for (int u : graph[v]) {
            reversed_graph[u].push_back(v);
        }
    }
    vector<int> sorted_vertexes;
    vector<int> used(n);
    for (int v = 0; v < n; ++v) {
        if (!used[v]) {
            topsort(graph, used, sorted_vertexes, v);
        }
    }
    reverse(sorted_vertexes.begin(), sorted_vertexes.end());
    component.assign(n, -1);
    component_graph.assign(n, vector<int>());
    int component_number = 0;
    for (int i = 0; i < n; ++i) {
        if (component[sorted_vertexes[i]] == -1) {
            find_one_component(reversed_graph, sorted_vertexes[i], component_number++);
            sort(component_graph[component_number - 1].begin(), component_graph[component_number - 1].end());
            component_graph[component_number - 1].resize(
                unique(component_graph[component_number - 1].begin(), component_graph[component_number - 1].end()) -
                component_graph[component_number - 1].begin());
        }
    }
}

void Game::topsort(vector<vector<int>>& graph, vector<int>& used, vector<int>& sorted_vertexes, int v) {
    used[v] = 1;
    for (int u : graph[v]) {
        if (!used[u]) {
            topsort(graph, used, sorted_vertexes, u);
        }
    }
    sorted_vertexes.push_back(v);
}

void Game::find_one_component(vector<vector<int>>& graph, int v, int component_number) {
    component[v] = component_number;
    for (int u : graph[v]) {
        if (component[u] == -1) {
            find_one_component(graph, u, component_number);
        } else {
            component_graph[component_number].push_back(component[u]);  // Not sure if we need this
        }
    }
}

void Game::generate(vector<Strategy> &s, vector<int>& strategy, int done_vertexes) {
    if (done_vertexes == g.size()) {
        s.emplace_back(strategy);
        return;
    }
    if (g[done_vertexes].empty()) {
        strategy[done_vertexes] = done_vertexes;
        generate(s, strategy, done_vertexes + 1);
    }
    for (auto neig : g[done_vertexes]) {
        strategy[done_vertexes] = neig;
        generate(s, strategy, done_vertexes + 1);
    }
}

vector<Strategy> Game::generate_strategies() {
    player_cnt = *max_element(player.begin(), player.end());
    vector<Strategy> strategies;
    vector<int> strategy(g.size());
    generate(strategies, strategy, 0);
}

vector<Strategy> Game::neighbour_strategies(Strategy strategy, int k) {
    vector<Strategy> ans;
    for (int i = 0; i < player.size();++i) {
        if (player[i] == k) {
            for (auto edge : g[i]) {
                if (edge != strategy[i]) {
                    ans.push_back(strategy.GetNewStrategy(i, edge));
                }
            }
        }
    }
    return ans;
}