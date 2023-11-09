#include "game.h"
#include <algorithm>
#include <iostream>

Game::Game(int n, int start): start(start), g(n), player(n), component(n), component_graph(n) {
}

void Game::add_edge(int from, int to) {
    g[from].push_back(to);
}

void Game::fill_components() {
    // Sorry guys, but I cannot do it another way
    vector<vector<int>> &graph = g;
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
        }
        sort(component_graph.begin(), component_graph.end());
        component_graph.resize(unique(component_graph.begin(), component_graph.end()) - component_graph.begin());
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
            component_graph[component_number].push_back(component[u]); // Not sure if we need this
        }
    }
}
