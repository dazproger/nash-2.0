#pragma once
#include <vector>
#include <iostream>

class Strategy {
public:
    Strategy(const std::vector<int>& strategy) {
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
    void print() const {
        std::cout << "Strategy: ";
        for (auto el : data_) {
            std::cout << el << " ";
        }
        std::cout << "\n";
    }
private:
    std::vector<int> data_;
};
