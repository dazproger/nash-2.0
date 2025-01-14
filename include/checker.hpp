#pragma once

#include <fstream>
#include <vector>
#include <iostream>

using std::cout;
using std::vector;

void stupid_check_skeleton(Game& g);
void smart_check_skeleton(Game& g, int closeness_to_playonce = 0);

void one_infinite_check_skeleton(Game& g, int closeness_to_playonce = 0);

class Checker 
{
public:
    // Function that initializes Checker from graphs/c22_contrexample
    void init();

    // Plays strategy represeneted by the given vector from starting_vertex to the end
    int find_strategy_outcome(const vector<int>& strategy) const;

    // Checks whether the player can get a better outcome for himself by changing the given strategy
    bool is_improvable_by_player(int player, const vector<int>& strategy) const;

    // Checks that the strategy is not a nash equililbrium
    void check_not_equilibrium(const vector<int>& strategy);

    // Recursively generates all strategies
    void strategies_generate(vector<int>& strategy, int current_vertex);

    // Recursively generates all changes that the player can apply to a strategy and puts them in possible_changes vector
    void generate_change(int player, vector<std::pair<int, int>>& change_prefix);

    // Fills possible_changes vector
    void make_changes();

    // Checks that the example given in graphs/c22_contrexample does not have a Nash equililbrium
    bool check();

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
};