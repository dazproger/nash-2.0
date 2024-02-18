#include <iostream>
#include "game.h"
#include "sat.h"

using namespace std;

// Recursive search of players; in g only the graph should be specified
void generate_players(Game& g) { } // TODO

int main() {
    int n;
    cout << "Input number of vertices: ";
    cin >> n;
    int start;
    cout << "Input starting vertex: ";
    cin >> start;
    --start;
    Game g(n, start);
    cout << "Input players corresponding to vertices: ";
    for (int i = 0; i < n; ++i) {
        int player;
        cin >> player;
        g.set_player(i, player - 1);
    }
    cout << "Input number of edges: ";
    int m;
    cin >> m;
    cout << "Input edges (m lines):\n";
    while (m--) {
        int a, b;
        cin >> a >> b;
        --a;--b;
        g.add_edge(a, b);
    }
    g.fill_components();
    SAT s(g);
    s.add_all_strategies(g);
    // g.print_components();
    // s.add_constraint(5, 2, 2);
    // s.print_beautiful_results(g);
    g.print_terminal_descriptions();
    s.print_all_beautiful_solutions();
    return 0;
}
