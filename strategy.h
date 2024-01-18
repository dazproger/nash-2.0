#pragma once

#include <vector>

using namespace std;
struct Strategy {
public:
	Strategy(const vector<int>& strategy){
        id = last_id;
        ++last_id;
        data_ = strategy;
    }
	int operator[] (int from){
        return data_[from];
    };
	int operator() (int from) {
        return data_[from];
    };
    int GetId(){
        return id;
    }
private:
	static inline int last_id=0;
	vector<int> data_;
	int id;
	int final_vertex; // Probably needs to be removed
};