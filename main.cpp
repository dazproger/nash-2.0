#include <iostream>
#include "game.h"

using namespace std;

int main() {
    int n;
    cin >> n;
    Game g(n, 0);
    int m;
    cin >> m;
    for (int i = 0; i < m; ++i) {
        int a, b;
        cin >> a >> b;
        --a; --b;
        g.add_edge(a, b);
    }
    g.fill_components();
    return 0;
}
