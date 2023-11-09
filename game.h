#pragma once

#include <vector>

#include "strategy.h"

using namespace std;

class Game {
public:
	Game(int, int);
	void add_edge(int, int);
	void set_player(int, int);
	void build();
	void fill_components();
private:
	vector<vector<int>> g;
	vector<int> player;
	vector<int> component;
	vector<vector<int>> component_graph;
	int start;
};
