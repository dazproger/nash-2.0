#pragma once

#include <vector>

#include "strategy.h"

using namespace std;

class Game {
public:
	Game(int, int);
	void add_edge(int, int); // Terminal should be looped into itself
	void set_player(int, int);
	void build();
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
