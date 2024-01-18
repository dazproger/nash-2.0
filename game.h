#pragma once

#include <vector>

#include "strategy.h"

using namespace std;

class Game {
public:
	Game(int, int);
	void add_edge(int, int);
	void set_player(int, int);
    vector<Strategy> generate_strategies();
    vector<Strategy> neighbour_strategies(Strategy);
    vector<int> get_terminal_components();
	void fill_components();
    void print_comps();
private:
    void topsort(vector<vector<int>>&, vector<int>&, vector<int>&, int);
    void find_one_component(vector<vector<int>>&, int, int);
	vector<vector<int>> g;
	vector<int> player;
	vector<int> component;
	vector<vector<int>> component_graph;
	int start;
};
