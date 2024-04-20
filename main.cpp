#include <iostream>
#include <ctime>
#include "game.h"
#include "sat.h"
#include <thread>

using namespace std;
int solve(const Game& g) {
    SAT initial_sat(g);
    initial_sat.add_all_strategies(g);
    if (!initial_sat.is_solvable()) {
        // cout << "\x1b[31;1mThere is a Nash Equilibirum((((\x1b[0m";
        return 0;
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

// Recursive search of players; in g only the graph should be specified
void generate_players(Game& g) {
    vector<int> pref;
    vector<int> used(g.get_vertices_count());
    rec(pref, used, 0, 0, g);
}

void check(Game game) {
    game.set_graph_info();
    generate_players(game);
}

int main(int argc, __attribute__((unused)) char* argv[]) {
    int n;
    if (argc <= 1)
        cout << "Input number of vertices: ";
    cin >> n;
    int start;
    if (argc <= 1)
        cout << "Input starting vertex: ";
    cin >> start;
    --start;
    const int amount_of_terminals = 6;
    int amount_of_games = 1 << amount_of_terminals;
    vector<Game> games;
    for (int i = 0; i < amount_of_games; ++i) {
        games.emplace_back(n - amount_of_terminals + __builtin_popcount(static_cast<uint>(i)), start);
    }
    if (argc <= 1)
        cout << "Input players corresponding to vertices: ";
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
        if (b < n - amount_of_terminals) {
            for (auto& el : games) {
                el.add_edge(a, b);
            }
        } else {
            auto exp = n - b - 1;
            for (int i = 0; i < amount_of_games; ++i) {
                if ((i >> exp) & 1) {
                    games[i].add_edge(a, n - amount_of_terminals + __builtin_popcount(static_cast<uint>(i >> exp)) - 1);
                }
            }
        }
    }
    // for (int i = 0; i< amount_of_games; ++i) {
    //     check(games[i]);
    // }
    auto st = time(nullptr);
    for (int i = 0; i < amount_of_games / 8; ++i) {
        vector<thread> threads;
        for (int j = 1; j <= 8; ++j) {
            if (8 * i + j < amount_of_games) {
                threads.emplace_back(check, games[8 * i + j]);
            }
        }
        for (auto& thread : threads) {
            thread.join();
        }
        cout << i << endl;
    }
    cout << difftime(time(nullptr), st) << endl;
    return 0;
}
