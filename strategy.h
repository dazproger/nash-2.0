
#pragma once
#include <unordered_set>
#include <vector>
static int used[100];
using namespace std;
struct Strategy {
public:
    Strategy(const vector<int>& strategy) {
        data_ = strategy;
    }
    int operator[](int from) const {
        return data_[from];
    }
    int operator()(int from) const {
        int cur = from;
        while (!used[from]) {
            used[from] = 1;
            from = data_[from];
        }
        while (used[cur]) {
            used[cur] = 0;
            cur = data_[cur];
        }
        return from;
    }
    Strategy GetNewStrategy(int index, int new_edge) const {
        auto data = data_;
        data[index] = new_edge;
        return data;
    }
private:
    vector<int> data_;
};
