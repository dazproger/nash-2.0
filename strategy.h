#pragma once

#include <vector>

using namespace std;
struct Strategy {
public:
	Strategy(const vector<int>& strategy){
        data_ = strategy;
    }
	int operator[] (int from){
        return data_[from];
    };
	int operator() (int from) {
        size_t cnt = 0;
        while (cnt < data_.size() + 10) {
            from = data_[from];
            ++cnt;
        }
        return from;
    };
    Strategy GetNewStrategy(int index, int new_edge) {
        auto data = data_;
        data[index] = new_edge;
        return data;
    }
private:
	vector<int> data_;
};