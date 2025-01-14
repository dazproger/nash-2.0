#pragma once

#include <vector>
#include <unordered_set>

#include "strategy.h"

using std::vector;

class Game 
{
public:
    Game(int n, int start);
    Game(const Game& g);
    Game(const std::vector<std::vector<int>>& graph_matrix, int start);

    void add_edge(int from, int to);
    void set_player(int i, int player_to_set);
    vector<Strategy> generate_strategies() const;
    vector<int> get_terminal_components() const;
    vector<Strategy> neighbour_strategies(const Strategy&, int) const;
    vector<int> neighbour_strategies_outcomes(const Strategy&, int) const;
    void fill_components();
    void print_components() const;
    void print_terminal_descriptions() const;
    int play_strat(const Strategy&) const;
    int get_player_count() const;
    int get_vertices_count() const;
    int get_components_count() const;
    int get_starting_vertex() const;
    vector<vector<int>> get_components() const;
    vector<int> get_cnt_components() const;
    vector<int> get_cycles() const;
    void set_graph_info();
    void reset_max_player();
    bool is_leaf(int) const;
    void print_graph() const;
private:
    vector<vector<int>> g; // graph vector of neighbours
    vector<int> player; // players corresponding to each vertex, first players corresponds to terminals(leaves)
    vector<int> component; // number of a component of double connectivity, in which lies vertex
    vector<vector<int>> component_graph; // graph of commponents
    int start;
    int player_cnt = 0;
    vector<int> cnt_components_; // amount of vertexes in each component
    vector<int> cycles; // list of components which are cycles <=> have more than one vertex <=> nonterminal outcomes
};
