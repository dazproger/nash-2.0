//
// Created by bogdan on 20.04.24.
//

#include <fstream>
#include <vector>
#include <iostream>

using std::cout;
using std::vector;

class Checker {
private:
    int vertices_count;            // Number of vertices in the given graph
    int players_count;             // Number of players, currently only 3
    int terminals_count;           // Number of terminal components
    int starting_vertex;           // The vertexes from which the game starts
    vector<vector<int>> graph;     // Given graph
    vector<int> vertex_component;  // vertex_component[v] is number of component of vertex v
    vector<vector<vector<bool>>>
        player_preference;      // player_preference[p][x][y] = 1 if and only if p prefers terminal x over terminal y
    vector<int> vertex_player;  // vertex_player[v] is number of player that controls this vertex
    vector<vector<int>> vertices_by_players;                  // Vertices grouped by players
    vector<vector<vector<std::pair<int, int>>>> possible_changes;  // Array of all possible ways how a strategy can change
    bool is_correct = true;  // Controls whether has Nash equililbrium been found or not

public:
    // Function that initializes Checker from graphs/c22_contrexample
    void init() {
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
    int find_strategy_outcome(const vector<int>& strategy) const {
        int current = 0;
        for (int i = 0; i < strategy.size() + 10; ++i) {
            current = strategy[current];
        }
        return vertex_component[current];
    }

    // Checks whether the player can get a better outcome for himself by changing the given strategy
    bool is_improvable_by_player(int player, const vector<int>& strategy) const {
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
    void check_not_equilibrium(const vector<int>& strategy) {
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
    void strategies_generate(vector<int>& strategy, int current_vertex) {
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
    void generate_change(int player, vector<std::pair<int, int>>& change_prefix) {
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
    void make_changes() {
        for (int player = 0; player < players_count; ++player) {
            vector<std::pair<int, int>> empty_prefix;
            generate_change(player, empty_prefix);
        }
    }

    // Checks that the example given in graphs/c22_contrexample does not have a Nash equililbrium
    bool check() {
        init();
        make_changes();
        vector<int> empty_strategy(vertices_count);
        strategies_generate(empty_strategy, 0);
        return is_correct;
    }
};

int main() {
    Checker checker;
    if (checker.check()) {
        cout << "OK.\n";
    } else {
        cout << "Something went wrong.\n";
    }
}
