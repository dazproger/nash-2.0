#include <iostream>
#include "game.h"
#include "sat.h"

using namespace std;

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
    g.print_components();
    //s.add_contraint(2, 3, 2);
    s.print_all_beautiful_solutions(g);
    return 0;
}
