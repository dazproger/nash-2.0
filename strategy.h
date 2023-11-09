#pragma once

#include <vector>

using namespace std;

struct Strategy {
public:
	Strategy(vector<int>);
	int operator[] (int from);
	int operator() (int from);
private:
	static int last_id;
	vector<int> data;
	int id;
	int final_vertex; // Probably needs to be removed
};
