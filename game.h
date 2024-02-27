#pragma once

#include <vector>
#include <unordered_set>

#include "strategy.h"

using namespace std;

class Game {
public:
	Game(int, int);
	void add_edge(int, int);
	void set_player(int, int);
    vector<Strategy> generate_strategies() const;
    vector<int> get_terminal_components() const;
    vector<Strategy> neighbour_strategies(const Strategy&, int) const;
    unordered_set<int> neighbour_strategies_outcomes(const Strategy&, int) const;
	void fill_components();
    void print_components() const;
    void print_terminal_descriptions() const;
    int play_strat(const Strategy&) const;
    int get_player_count() const;
    int get_vertices_count() const;
    int get_components_count() const;
    vector<vector<int>> get_components() const;
    vector<int> get_cnt_components() const;
    vector<int> get_cycles() const;
    void set_graph_info();
    void reset_max_player();
    bool is_leaf(int) const;
private:
	vector<vector<int>> g;
	vector<int> player;
	vector<int> component;
	vector<vector<int>> component_graph;
	int start;
    int player_cnt = 0;
    vector<int> cnt_components_;
    vector<int> cycles;
};
