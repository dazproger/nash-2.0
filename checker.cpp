#include "game.h"
#include "sat.h"

void set_play_once_players(Game& g) {
    int n = g.get_vertices_count();
    int cnt = 1;
    g.set_player(0, 0);
    for(int i = 1; i < n; ++i) {
        if (g.is_leaf(i)) {
            g.set_player(i, 0);
        }
        else {
            g.set_player(i, cnt);
            ++cnt;
        }
    }
}

void stupid_check_skeleton(Game& g) {
    set_play_once_players(g);
    SAT initial_sat(g);
    initial_sat.add_all_strategies(g);
    if (!initial_sat.is_solvable()) {
        // cout << "\x1b[31;1mThere is a Nash Equilibirum((((\x1b[0m";
    } else {
        initial_sat.print_beautiful_results();
        cout << "hell yeah\n";
    }
}

void rec(vector<int>& pref, vector<int>& used, int cnt_used, int num_last, Game& g) {
    if (pref.size() == g.get_vertices_count()) {
        if (cnt_used <= 2)
            return;
        for (int v = 0; v < g.get_vertices_count(); ++v) {
            g.set_player(v, pref[v]);
        }
        SAT initial_sat(g);
        initial_sat.add_all_strategies(g);
        if (!initial_sat.is_solvable()) {
            // cout << "\x1b[31;1mThere is a Nash Equilibirum((((\x1b[0m";
        } else {
            initial_sat.print_beautiful_results();
            cout << "hell yeah\n";
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

void tester_check_skeleton(Game& g) {
    vector<int> used(g.get_vertices_count());
    vector<int> pref;
    rec(pref, used, 0, 0, g);
}

// works only for one whole infinite outcome
void smart_check_skeleton(Game& g) {
    set_play_once_players(g);
    // go threw some предпочтения check is there Nash equilibruim
}