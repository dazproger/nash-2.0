#include <iostream>
#include "game.h"
#include "sat.h"

using namespace std;

int solve(const Game& g) {
    SAT initial_sat(g);
    initial_sat.add_all_strategies(g);
    if (!initial_sat.is_solvable()) {
        // cout << "\x1b[31;1mThere is a Nash Equilibirum((((\x1b[0m";
        return 0;
    }
    for (int i = 0; i < g.get_player_count(); ++i) {
        for (const auto& cycle : g.get_cycles()) {
            SAT s(g);
            s.add_all_strategies(g);
            s.minimize_all_except(cycle, i);
            if (s.is_solvable()) {
                cout << "\x1b[32;1mOH MY GOD YES WE HAVE FOUND IT!!!!!\x1b[0m\n";
                s.print_beautiful_results();
                exit(0);
            }
        }
    }
    // cout << "\x1b[31;1m((\x1b[0m" << endl;
    // initial_sat.print_all_solutions_close_to_c22();
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
        if (solve(g)) {
            for (auto elem : pref) {
                cout << elem + 1 << ' ';
            }
            cout << endl;
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
        rec(pref, used, cnt_used, num_last + (player == num_last), g);
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
    Game g(n, start);
    // if (argc <= 1)
    //     cout << "Input players corresponding to vertices: ";
    if (argc <= 1)
        cout << "Input number of edges: ";
    int m;
    cin >> m;
    if (argc <= 1)
        cout << "Input edges (m lines):\n";
    while (m--) {
        int a, b;
        cin >> a >> b;
        --a;
        --b;
        g.add_edge(a, b);
    }
    g.set_graph_info();
    g.print_terminal_descriptions();
    generate_players(g);
    g.print_terminal_descriptions();
    return 0;
}
