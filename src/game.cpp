#include "game.h"
#include <algorithm>
#include <iostream>

Game::Game(int n, int start) 
: g(n), player(n), component(n), component_graph(n), start(start) 
{}

Game::Game(const std::vector<std::vector<int>>& matrix, int start) 
: g(matrix), player(matrix.size()), component(matrix.size()), component_graph(matrix.size()), start(start) 
{}

Game::Game(const Game& other) {
    g = other.g;
    player = other.player;
    component = other.component;
    component_graph = other.component_graph;
    start = other.start;
}

void Game::add_edge(int from, int to) {
    g[from].push_back(to);
}

void Game::set_player(int i, int player_to_set) {
    player[i] = player_to_set;
    player_cnt = std::max(player_to_set + 1, player_cnt);
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

static void find_one_component(const vector<vector<int>>& graph, vector<int>& component,
                               vector<vector<int>>& component_graph, int v, int component_number) {
    component[v] = component_number;
    for (int u : graph[v]) 
    {
        if (component[u] == -1)
            find_one_component(graph, component, component_graph, u, component_number);
        else if (component[u] != -2) { // Forbidden component - look at Game::neighbour_strategies_outcomes
            component_graph[component[u]].push_back(component_number);
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
        if (component[sorted_vertexes[i]] == -1) 
        {
            find_one_component(reversed_graph, component, component_graph, sorted_vertexes[i], component_number);
            ++component_number;
        }
    }
    component_graph.resize(component_number);
    for (int i = 0; i < component_number; ++i) {
        make_unique(component_graph[i]);
        if (component_graph[i].empty())
            component_graph[i].push_back(i);
    }
}

void Game::print_terminal_descriptions() const {
    vector<vector<int>> components = get_components(); // Vertices grouped by components
    std::cout << "Terminals description:\n";
    vector<int> my_terminals = get_terminal_components();
    for (size_t i = 0; i < my_terminals.size(); ++i) {
        std::cout << "Terminal number " << i + 1 << " contains vertexes:\n";
        for (auto el : components[my_terminals[i]])
            std::cout << el + 1 << " ";

        if (components[my_terminals[i]].size() > 1)
        {
            std::cout << "\x1b[32;1m";  // Print in bold green
            std::cout << " IT'S A CYCLE (component: " << my_terminals[i] << ")";
            std::cout << "\x1b[0m";  // Reset color
        }
        std::cout << "\n";
    }
}

void Game::print_components() const {
    vector<vector<int>> components(get_components_count());
    for (int i = 0; i < get_vertices_count(); ++i)
        components[component[i]].push_back(i);

    for (int i = 0; i < get_components_count(); ++i) {
        std::cout << "Component " << i << ": ";
        for (auto j = 0LU; j < components[i].size(); ++j) {
            if (j)
                std::cout << ", ";
            std::cout << components[i][j];
        }
        std::cout << '\n';
    }
}

vector<int> Game::get_terminal_components() const {
    vector<int> cnt_components = cnt_components_;
    vector<int> is_terminal(get_components_count());

    for (int i = 0; i < get_components_count(); ++i) {
        is_terminal[i] = cnt_components[i] > 2;
        for (int destination_component : component_graph[i])
            is_terminal[i] |= (destination_component == i);
    }

    vector<int> terminals;
    for (int i = 0; i < get_components_count(); ++i)
        if (is_terminal[i])
            terminals.push_back(i);

    return terminals;
}

static void generate(const vector<vector<int>>& g, vector<Strategy>& s, vector<int>& strategy, int done_vertexes)
{
    if (done_vertexes == (int)g.size()) {
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

vector<Strategy> Game::neighbour_strategies(const Strategy& strategy, int k) const {
    vector<Strategy> ans;
    for (int i = 0; i < get_vertices_count(); ++i) {
        if (player[i] == k) {
            size_t n = ans.size();
            for (auto edge : g[i])
                if (edge != strategy[i])
                    ans.push_back(strategy.GetNewStrategy(i, edge));

            for (size_t j = 0; j < n; ++j) {
                for (auto edge : g[i]) {
                    if (edge != ans[j][i])
                        ans.push_back(ans[j].GetNewStrategy(i, edge));
                }
            }
        }
    }
    return ans;
}
vector<int> Game::neighbour_strategies_outcomes(const Strategy& strategy, int k) const {
    int n = get_vertices_count();
    vector<int> outcomes;
    int cur_player = k;

    // Graph creation
    vector<vector<int>> left_graph(n);
    vector<vector<int>> reversed_graph(n);
    for (int v = 0; v < n; ++v) {
        if (player[v] != cur_player) {
            left_graph[v].push_back(strategy[v]);
            reversed_graph[strategy[v]].push_back(v);
            continue;
        }
        copy(g[v].begin(), g[v].end(), back_inserter(left_graph[v]));
        for (int u : g[v])
            reversed_graph[u].push_back(v);
    }

    // Topologically sorting
    vector<int> sorted_vertexes;
    vector<int> used(n, 0);
    topsort(left_graph, used, sorted_vertexes, start);

    // Finding components
    reverse(sorted_vertexes.begin(), sorted_vertexes.end());
    vector<int> left_component(n, -1);
    for (auto v = 0LU; v < used.size(); ++v) { // All vertices not reachable from start are in one component. now find_one_component will not find them
        if (!used[v]) {
            left_component[v] = -2;
        }
    }
    vector<vector<int>> left_component_graph(n, vector<int>());
    int component_number = 0;
    for (auto i = 0LU; i < sorted_vertexes.size(); ++i) {
        if (left_component[sorted_vertexes[i]] == -1) {
            find_one_component(reversed_graph, left_component, left_component_graph, sorted_vertexes[i], component_number++);
        }
    }
    left_component_graph.resize(component_number);
    for (int i = 0; i < component_number; ++i) {
        make_unique(left_component_graph[i]);
        if (left_component_graph[i].empty())
            left_component_graph[i].push_back(i);
    }

    // Filling left_cnt_components
    vector<int> left_cnt_components(component_number);
    for (int v = 0; v < n; ++v) {
        if (!used[v]) continue;
        ++left_cnt_components[left_component[v]];
    }

    // Adding outcomes
    for (int v = 0; v < n; ++v) {
        if (!used[v]) continue;
        int c_left = left_component[v];

        if (left_cnt_components[c_left] > 1) {
            outcomes.push_back(component[v]);
            continue;
        }
        for (int u : left_component_graph[c_left]) {
            if (u == c_left) {
                outcomes.push_back(component[v]);

                break;
            }
        }
    }
    make_unique(outcomes);

    vector<int> ans;
    int initial_outcome = play_strat(strategy);
    for (auto el : outcomes) {
        if (el != initial_outcome) {
            ans.push_back(el);
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

int Game::get_starting_vertex() const {
    return start;
}

int Game::get_player_count() const {
    return player_cnt;
}

vector<vector<int>> Game::get_components() const {
    vector<vector<int>> vertices_grouped_by_components(get_components_count());
    for (int i = 0; i < get_vertices_count(); ++i) {
        vertices_grouped_by_components[component[i]].push_back(i);
    }
    return vertices_grouped_by_components;
}

vector<int> Game::get_cnt_components() const {
    vector<int> cnt_components(get_components_count());
    for (int i = 0; i < get_vertices_count(); ++i) {
        ++cnt_components[component[i]];
    }
    return cnt_components;
}

int Game::play_strat(const Strategy& strat) const {
    return component[strat(start)];
}

vector<int> Game::get_cycles() const {
    return cycles;
}

void Game::reset_max_player() {
    player_cnt = 0;
}

void Game::set_graph_info() {
    fill_components();
    cnt_components_ = get_cnt_components();
    for (int i = 0; i < get_components_count(); ++i) {
        if (cnt_components_[i] > 1) {
            cycles.push_back(i);
        }
    }
}

bool Game::is_leaf(int v) const {
    return component_graph[component[v]].size() == 1 && component_graph[component[v]][0] == component[v];
}

void Game::print_graph() const {
    for (size_t i = 0; i < g.size();++i) {
        for (auto el : g[i]) {
            std::cout << i + 1 << ' ' << el+ 1 << '\n';
        }
    }
    for (auto el : player) {
        std::cout << el + 1 << ' ';
    }
    std::cout << '\n';
    print_terminal_descriptions();
}
