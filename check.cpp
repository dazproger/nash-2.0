//
// Created by bogdan on 20.04.24.
//

#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

class Checker {
  private:
    int players_num;
    int n;
    int start;
    int edges_count;
    vector<vector<int>> g;
    int terminal_count;
    vector<int> cmp_v;
    vector<vector<vector<int>>> priority;
    vector<int> players_arr;
    vector<vector<int>> players_v;
    vector<vector<vector<pair<int, int>>>> changes;
    int is_correct = 1;
  public:
    void init() {
        std::fstream in;
        in.open ("/home/bogdan/nash-equilibrium/graphs/c22_contrexample", std::fstream::in);
        players_num = 3;
        in >> n;
        in >> start;
        --start;
        in >> edges_count;
        g.assign(n, {});
        for (int i = 0; i < edges_count; ++i) {
            int x, y;
            in >> x >> y;
            --x;
            --y;
            g[x].push_back(y);
        }
        for (int i = 0; i < n; ++i) {
            if (g[i].empty()) {
                g[i].push_back(i);
            }
        }
        in >> terminal_count;
        cmp_v.assign(n, -1);
        for (int i = 0; i < terminal_count; ++i) {
            int cnt;
            in >> cnt;
            for (int j = 0; j < cnt; ++j) {
                int x;
                in >> x;
                --x;
                cmp_v[x] = i;
            }
        }
        priority.assign(players_num, vector<vector<int>>(terminal_count, vector<int>(terminal_count, 0)));
        for (int p = 0; p < players_num; ++p) {
            vector<int> pr;
            for (int i = 0; i < terminal_count; ++i) {
                int t;
                in >> t;
                --t;
                pr.push_back(t);
            }
            for (int i = 0; i + 1 < pr.size(); ++i) {
                for (int j = i + 1; j < pr.size(); ++j) {
                    priority[p][pr[i]][pr[j]] = 1;
                    priority[p][pr[j]][pr[i]] = -1;
                }
            }
        }
        players_arr.assign(n, 0);
        players_v.assign(players_num, vector<int>());
        for (int i = 0; i < n; ++i) {
            in >> players_arr[i];
            --players_arr[i];
            players_v[players_arr[i]].push_back(i);
        }
        changes.resize(players_num);
        in.close();
    }

    int get_end_cmp(const vector<int>& strategy) {
        int cur = 0;
        for (int i = 0; i < strategy.size() + 10; ++i) {
            cur = strategy[cur];
        }
        return cmp_v[cur];
    }

    int can_make_better(int p, vector<int>& strategy, int cmp) {
        for (auto& change : changes[p]) {
            vector<int> new_strategy = strategy;
            for (auto pr : change) {
                new_strategy[pr.first] = pr.second;
            }
            int new_cmp = get_end_cmp(new_strategy);
            if (priority[p][cmp][new_cmp] == 1) {
                return 1;
            }
        }
        return 0;
    }

    void check_no_equilibrium(vector<int>& strategy, int cmp) {
        int cnt = 0;
        for (int p = 0; p < players_num; ++p) {
            cnt += can_make_better(p, strategy, cmp);
        }
        if (cnt == 0) {
            is_correct = 0;
            cout << endl;
            for (auto x : strategy) {
                cout << x + 1 << " ";
            }
            cout << endl;
        }
    }

    void generate(vector<int>& arr, int cur_num) {
        if (cur_num == arr.size()) {
            int cmp = get_end_cmp(arr);
            check_no_equilibrium(arr, cmp);
            return;
        }
        for (auto u : g[cur_num]) {
            arr[cur_num] = u;
            generate(arr, cur_num + 1);
        }
    }

    void generate_change(int p, vector<pair<int, int>>& cur, int cur_num) {
        if (cur_num == players_v[p].size()) {
            changes[p].push_back(cur);
            return;
        }
        for (auto u : g[players_v[p][cur_num]]) {
            cur.emplace_back(players_v[p][cur_num], u);
            generate_change(p, cur, cur_num + 1);
            cur.pop_back();
        }
    }

    void make_changes() {
        for (int p = 0; p < players_num; ++p) {
            vector<pair<int, int>> cur;
            generate_change(p, cur, 0);
        }
    }

    int check() {
        init();
        make_changes();
        vector<int> arr(n, 0);
        generate(arr, 0);
        return is_correct;
    }
};


int main() {
    Checker checker;
    if (checker.check()) {
        cout << "OK.\n";
    } else {
        cout << "Something went wrong.\n";
    }
}