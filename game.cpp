#include "game.h"
#include <algorithm>

Game::Game(int n, int start) : g(n), player(n), component(n), component_graph(n), start(start) {
}

void Game::add_edge(int from, int to) {
    g[from].push_back(to);
}

void Game::set_player(int i, int player) {
    this -> player[i] = player;
    player_cnt = max(player, player_cnt);
}

static void topsort(const vector<vector<int>>& graph, vector<int>& used, vector<int>& sorted_vertexes, int v) {
    used[v] = 1;
    for (int u : graph[v]) {
        if (!used[u]) {
            topsort(graph, used, sorted_vertexes, u);
        }
    }
    sorted_vertexes.push_back(v);
}

static void find_one_component(const vector<vector<int>>& graph, vector<int>& component, vector<vector<int>>& component_graph, int v, int component_number) {
    component[v] = component_number;
    for (int u : graph[v]) {
        if (component[u] == -1) {
            find_one_component(graph, component, component_graph, u, component_number);
        } else {
            component_graph[component_number].push_back(component[u]); 
        }
    }
}

static void make_unique(vector<int>& vec) {
    sort(vec.begin(), vec.end());
    vec.resize(unique(vec.begin(), vec.end()) - vec.begin());
}

void Game::fill_components() {
    vector<vector<int>>& graph = g; // Rename g to graph in this function
    int n = get_vertices_count();
    vector<vector<int>> reversed_graph(n);
    for (int v = 0; v < n; ++v) {
        for (int u : graph[v]) {
            reversed_graph[u].push_back(v);
        }
    }
    vector<int> sorted_vertexes;
    vector<int> used(n, 0);
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
            find_one_component(reversed_graph, component, component_graph, sorted_vertexes[i], component_number);
            make_unique(component_graph[component_number]);
            ++component_number;
        }
    }
    component_graph.resize(component_number);
}



vector<int> Game::get_terminal_components() const {
    int n_comps = get_components_count(); // Number of components
    int n_verts = get_vertices_count(); // Number of vertexes
    vector<int> cnt_components(n_comps); // cnt_comoponents[i] - number of vertexes in comoponent i
    for (int i = 0; i < n_verts; ++i) {
        ++cnt_components[component[i]];
    }
    vector<int> is_terminal(n_comps);
    for (int i = 0; i < n_comps; ++i) {
        is_terminal[i] = cnt_components[i] > 2;
        for (int destination_component : component_graph[i]) {
            is_terminal[i] |= (destination_component == i);
        }
    }
    return is_terminal;
}

static void generate(const vector<vector<int>>& g, vector<Strategy> &s, vector<int>& strategy, int done_vertexes) {
    if (done_vertexes == g.size()) {
        s.emplace_back(strategy);
        return;
    }
    if (g[done_vertexes].empty()) {
        strategy[done_vertexes] = done_vertexes;
        generate(g, s, strategy, done_vertexes + 1);
    }
    for (auto neig : g[done_vertexes]) {
        strategy[done_vertexes] = neig;
        generate(g, s, strategy, done_vertexes + 1);
    }
}

vector<Strategy> Game::generate_strategies() const {
    vector<Strategy> strategies;
    vector<int> strategy(get_vertices_count());
    generate(g, strategies, strategy, 0);
    return strategies;
}

vector<Strategy> Game::neighbour_strategies(Strategy strategy, int k) const {
    vector<Strategy> ans;
    for (int i = 0; i < get_player_count();++i) {
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

int Game::get_vertices_count() const {
    return g.size();
}

int Game::get_components_count() const {
    return component_graph.size();
}
int Game::get_player_count() const {
    return player_cnt;
}

int Game::play_strat(const Strategy& strat) const {
    return component[strat(start)];
}
