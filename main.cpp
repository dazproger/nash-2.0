#include <iostream>
#include <ctime>
#include "game.h"
#include "sat.h"
#include "graph-iter.h"
#include <thread>

using std::cout;
using std::cin;
using std::endl;


int solve(const Game& g) {
    SAT initial_sat(g);
    initial_sat.add_all_strategies(g);
    if (!initial_sat.is_solvable()) {
        // cout << "\x1b[31;1mThere is a Nash Equilibirum((((\x1b[0m";
        return 0;
    }
    else{
        initial_sat.print_beautiful_results();
        cout << "hell yeah\n";
        return 1;
    }
    SAT start_sat(g);
    start_sat.add_all_strategies(g);
    for (int i = 0; i < g.get_player_count(); ++i) {
        for (const auto& cycle : g.get_cycles()) {
            SAT s = start_sat;  // Pls God protect this line of code
            s.minimize_all_except(cycle, i);
            if (s.is_solvable()) {
                cout << "\x1b[32;1mOH MY GOD YES WE HAVE FOUND IT!!!!!\x1b[0m" << endl;
                g.print_graph();
                s.print_beautiful_results();
                exit(0);
            }
        }
    }
    print_all_achieve_ranks({2, 2, 0}, g);
    // g.print_terminal_descriptions();
    // s.minimize_loop_rank(2, 2);
    // s.minimize_all_except(6, 2);
    // s.print_beautiful_results();
    // s.print_all_beautiful_solutions();
    return 1;
}

void rec(vector<int>& pref, vector<int>& used, int cnt_used, int num_last, Game& g) {
    if (pref.size() == g.get_vertices_count()) {
        if (cnt_used <= 2)
            return;
        for (int v = 0; v < g.get_vertices_count(); ++v) {
            g.set_player(v, pref[v]);
        }
        // print_all_achieve_ranks({2, 2, 0}, g);
        // g.reset_max_player();
        // return;
        if (solve(g)) {
            // for (auto elem : pref) {
            //     cout << elem + 1 << ' ';
            // }
            // cout << endl;
            // cout << "LOOK
            // UP\n//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n";
            cout << "solved \n";
        }
        g.reset_max_player();
        return;
    }
    if (g.is_leaf(pref.size())) {
        pref.push_back(0);
        rec(pref, used, cnt_used, num_last + (0 == num_last), g);
        pref.pop_back();
        return;
    }
    for (int player = 0; player <= num_last; ++player) {
        pref.push_back(player);
        cnt_used += used[player] == 0;
        used[player]++;
        rec(pref, used, cnt_used, num_last + (player == num_last) - (num_last == 3), g);
        used[player]--;
        cnt_used -= used[player] == 0;
        pref.pop_back();
    }
}

void check(Game g) {
    return;
}

bool test_new_func(Game& game) {
    auto all_strategies = game.generate_strategies();
    int k = game.get_player_count();
    for (const auto& strategy : all_strategies) {
        for (int i = 0; i < k; ++i) {
            auto fast_outcomes = game.neighbour_strategies_outcomes(strategy, k);
            vector<int> right_outcomes;
            int outcome = game.play_strat(strategy);
            for (const auto &neighbour_strat : game.neighbour_strategies(strategy, k)) {
                int other_outcome = game.play_strat(neighbour_strat);
                if (other_outcome != outcome) {
                    right_outcomes.push_back(other_outcome);
                }
            }
            std::sort(fast_outcomes.begin(), fast_outcomes.end());
            std::sort(right_outcomes.begin(), right_outcomes.end());
            if (right_outcomes != fast_outcomes) {
                strategy.print();
                cout << "player " << i << "\n";
                cout << "fast_outcomes:  ";
                for (auto el : fast_outcomes) {
                    cout << el << " ";
                }
                cout << '\n';
                cout << "right_outcomes: ";
                for (auto el : right_outcomes) {
                    cout << el << " ";
                }
                cout << '\n';
                return false;
            }
        }
    }
    return true;
}

void gen_players2(vector<int>& pref, vector<int>& used, int cnt_used, int num_last, Game& g) {
    if (pref.size() == g.get_vertices_count()) {
        if (cnt_used <= 2)
            return;
        for (int v = 0; v < g.get_vertices_count(); ++v) {
            g.set_player(v, pref[v]);
        }
        // print_all_achieve_ranks({2, 2, 0}, g);
        // g.reset_max_player();
        // return;
        if (!test_new_func(g)) {
            // for (auto elem : pref) {
            //     cout << elem + 1 << ' ';
            // }
            // cout << endl;
            // cout << "LOOK
            // UP\n//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n";
            g.print_graph();
            g.print_components();
            exit(0);
        }
        g.reset_max_player();
        return;
    }
    if (g.is_leaf(pref.size())) {
        pref.push_back(0);
        gen_players2(pref, used, cnt_used, num_last + (0 == num_last), g);
        pref.pop_back();
        return;
    }
    for (int player = 0; player <= num_last; ++player) {
        pref.push_back(player);
        cnt_used += used[player] == 0;
        used[player]++;
        gen_players2(pref, used, cnt_used, num_last + (player == num_last) - (num_last == 3), g);
        used[player]--;
        cnt_used -= used[player] == 0;
        pref.pop_back();
    }
}

Game read_graph(int argc) {
    int n;
    if (argc <= 1)
        cout << "Input number of vertices: ";
    cin >> n;
    int start;
    if (argc <= 1)
        cout << "Input starting vertex: ";
    cin >> start;
    --start;
    if (argc <= 1)
        cout << "Input number of edges: ";
    int m;
    cin >> m;
    if (argc <= 1)
        cout << "Input edges (m lines):\n";
    Game g(n, start);
    while (m--) {
        int a, b;
        cin >> a >> b;
        --a;
        --b;
        g.add_edge(a, b);
    }
    //g.set_graph_info()
    return g;
}
#define MAXN 10
#define MAXM 10
#include "nauty.h" // Основной заголовочный файл Nauty

int main(int argc, __attribute__((unused)) char* argv[]) {
    cout << "dont enter numbers higher than 6\nFOR REAL!!!!\n";
    int n;
    cin >> n;
    graph_bruteforce(n);
}

// mkdit build
// cmake --build ./build/ --target nash-2.0
// ./build/nash-2.0