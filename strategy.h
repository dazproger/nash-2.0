
#pragma once
#include <unordered_set>
#include <vector>

using std::vector;

struct Strategy {
public:
    Strategy(const vector<int>& strategy) {
        data_ = strategy;
    }
    int operator[](int from) const {
        return data_[from];
    }
    int operator()(int from) const {
        int USED[100] = {};
        int cur = from;
        while (!USED[from]) {
            USED[from] = 1;
            from = data_[from];
        }
        while (USED[cur]) {
            USED[cur] = 0;
            cur = data_[cur];
        }
        return from;
    }
    [[nodiscard]] Strategy GetNewStrategy(int index, int new_edge) const {
        auto data = data_;
        data[index] = new_edge;
        return data;
    }
private:
    vector<int> data_;
};
