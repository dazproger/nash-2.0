#include "game.h"
#include "sat.h"
#include "checker.hpp"

using std::cout;

void set_play_once_players(Game& g);
void stupid_check_skeleton(Game& g);
void smart_check_skeleton(Game& g);

void set_play_once_players(Game& g) {
    int n = g.get_vertices_count();
    int cnt = 1;
    g.set_player(0, 0);
    for(int i = 1; i < n; ++i) {
        if (g.is_leaf(i)) {
            g.set_player(i, 0);
        }
        else {
            g.set_player(i, cnt);
            ++cnt;
        }
    }
}

void stupid_check_skeleton(Game& g) {
    set_play_once_players(g);
    SAT initial_sat(g);
    initial_sat.add_all_strategies(g);
    if (!initial_sat.is_solvable()) {
        // cout << "\x1b[31;1mThere is a Nash Equilibirum((((\x1b[0m";
    } else {
        g.print_graph();
        initial_sat.print_beautiful_results();
        cout << "hell yeah\n";
        exit(0);
    }
}

// works only for one whole infinite outcome
void smart_check_skeleton(Game& g) {
    set_play_once_players(g);
    // go threw some предпочтения check is there Nash equilibruim
}

// Function that initializes Checker from graphs/c22_contrexample
void Checker::init() {
    std::ifstream in("graphs/c22_contrexample");
    players_count = 3;
    ////////////////////////// Graph input //////////////////////////
    in >> vertices_count;
    in >> starting_vertex;
    --starting_vertex;
    int edges_count;  // Number of edges in the graph
    in >> edges_count;
    graph.assign(vertices_count, {});
    for (int i = 0; i < edges_count; ++i) {
        int from, to;
        in >> from >> to;
        --from;
        --to;
        graph[from].push_back(to);
    }
    // If a vertex is a leaf we make it a loop
    for (int i = 0; i < vertices_count; ++i) {
        if (graph[i].empty()) {
            graph[i].push_back(i);
        }
    }
    ///////////////////// Terminals info input //////////////////////
    in >> terminals_count;
    vertex_component.assign(vertices_count, -1);
    for (int i = 0; i < terminals_count; ++i) {
        int vertices_in_terminal_count;
        in >> vertices_in_terminal_count;
        for (int j = 0; j < vertices_in_terminal_count; ++j) {
            int vertex;
            in >> vertex;
            --vertex;
            vertex_component[vertex] = i;
        }
    }
    ///////////////// Setting up player preference //////////////////
    player_preference.assign(players_count, vector<vector<bool>>(terminals_count, vector<bool>(terminals_count)));
    for (int p = 0; p < players_count; ++p) {
        vector<int> pr;
        for (int i = 0; i < terminals_count; ++i) {
            int t;
            in >> t;
            --t;
            pr.push_back(t);
        }
        for (int i = 0; i + 1 < pr.size(); ++i) {
            for (int j = i + 1; j < pr.size(); ++j) {
                player_preference[p][pr[i]][pr[j]] = true;
                player_preference[p][pr[j]][pr[i]] = false;
            }
        }
    }
    ///////////////////////// Players input /////////////////////////
    vertex_player.assign(vertices_count, 0);
    vertices_by_players.assign(players_count, vector<int>());
    for (int i = 0; i < vertices_count; ++i) {
        in >> vertex_player[i];
        --vertex_player[i];
        vertices_by_players[vertex_player[i]].push_back(i);
    }
    possible_changes.resize(players_count);
    in.close();
}

// Plays strategy represeneted by the given vector from starting_vertex to the end
int Checker::find_strategy_outcome(const vector<int>& strategy) const {
    int current = 0;
    for (int i = 0; i < strategy.size() + 10; ++i) {
        current = strategy[current];
    }
    return vertex_component[current];
}

// Checks whether the player can get a better outcome for himself by changing the given strategy
bool Checker::is_improvable_by_player(int player, const vector<int>& strategy) const {
    int outcome = find_strategy_outcome(strategy);
    for (const auto& change : possible_changes[player]) {
        vector<int> new_strategy = strategy;
        for (const auto &[vertex, neighbour] : change) {
            new_strategy[vertex] = neighbour;
        }
        int new_outcome = find_strategy_outcome(new_strategy);
        if (player_preference[player][outcome][new_outcome] == 1) {
            return true;
        }
    }
    return false;
}

// Checks that the strategy is not a nash equililbrium
void Checker::check_not_equilibrium(const vector<int>& strategy) {
    for (int player = 0; player < players_count; ++player) {
        if (is_improvable_by_player(player, strategy))
            return;
    }
    is_correct = false;
    cout << std::endl;
    for (const auto& element : strategy) {
        cout << element + 1 << " ";
    }
    cout << std::endl;
}

// Recursively generates all strategies
void Checker::strategies_generate(vector<int>& strategy, int current_vertex) {
    if (current_vertex == strategy.size()) {
        check_not_equilibrium(strategy);
        return;
    }
    for (const auto& neighbour : graph[current_vertex]) {
        strategy[current_vertex] = neighbour;
        strategies_generate(strategy, current_vertex + 1);
    }
}

// Recursively generates all changes that the player can apply to a strategy and puts them in possible_changes vector
void Checker::generate_change(int player, vector<std::pair<int, int>>& change_prefix) {
    int current_vertex_number = change_prefix.size();
    if (current_vertex_number == vertices_by_players[player].size()) {
        possible_changes[player].push_back(change_prefix);
        return;
    }
    for (const auto& neighbour : graph[vertices_by_players[player][current_vertex_number]]) {
        change_prefix.emplace_back(vertices_by_players[player][current_vertex_number], neighbour);
        generate_change(player, change_prefix);
        change_prefix.pop_back();
    }
}

// Fills possible_changes vector
void Checker::make_changes() {
    for (int player = 0; player < players_count; ++player) {
        vector<std::pair<int, int>> empty_prefix;
        generate_change(player, empty_prefix);
    }
}

// Checks that the example given in graphs/c22_contrexample does not have a Nash equililbrium
bool Checker::check() {
    init();
    make_changes();
    vector<int> empty_strategy(vertices_count);
    strategies_generate(empty_strategy, 0);
    return is_correct;
}