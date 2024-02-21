#pragma once

#include <vector>

#include "strategy.h"

using namespace std;

class Game {
public:
	Game(int, int);
	void add_edge(int, int);
	void set_player(int, int);
    vector<Strategy> generate_strategies() const;
    vector<int> get_terminal_components() const;
    vector<Strategy> neighbour_strategies(Strategy, int) const;
	void fill_components();
    void print_components() const;
    void print_terminal_descriptions() const;
    int play_strat(const Strategy&) const;
    int get_player_count() const;
    int get_vertices_count() const;
    int get_components_count() const;
    vector<vector<int>> get_components() const;
    vector<int> get_cnt_components() const;
    void set_graph_info();
private:
	vector<vector<int>> g;
	vector<int> player;
	vector<int> component;
	vector<vector<int>> component_graph;
	int start;
    int player_cnt = 0;
    vector<int> cnt_components;
    vector<int> cycles;
};
