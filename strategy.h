#pragma once

#include <vector>

using namespace std;

struct Strategy {
public:
    Strategy(const vector<int>& strategy) {
        data_ = strategy;
    }
    int operator[](int from) const {
        return data_[from];
    };
    int operator()(int from) const {
        size_t cnt = 0;
        while (cnt < data_.size() + 10) {
            from = data_[from];
            ++cnt;
        }
        return from;
    };
    Strategy GetNewStrategy(int index, int new_edge) const {
        auto data = data_;
        data[index] = new_edge;
        return data;
    }

private:
    vector<int> data_;
};
