#include "game.h"
#include "sat.h"

using std::cout;

void set_play_once_players(Game& g);
void stupid_check_skeleton(Game& g);
void smart_check_skeleton(Game& g);

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


// works only for one whole infinite outcome
void smart_check_skeleton(Game& g) {
    set_play_once_players(g);
    // go threw some предпочтения check is there Nash equilibruim
}