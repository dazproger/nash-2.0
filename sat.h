#include "strategy.h"
#include "game.h"
#include <ortools/sat/cp_model.h>

using namespace operations_research::sat;
using namespace std;

class SAT {
public:
    // Creates SAT by Game. After this constructor SAT should already have all X_ijk variables created and all triangle restrictions should be added.
    // Don't forget to name the variables
    SAT(const Game&);
    // Adds strategy restriction to the SAT
    void add_strategy(const Strategy&, const Game&);
    // Adds all strategies from the game
    void add_all_strategies(const Game&);
    // Solves the SAT and prints the results
    void solve();
    // Should add another function that extracts data from solved SAT
    // Returns the X_ijk Boolean variable
    BoolVar get_var(int i, int j, int k);
private:
    CpModelBuilder cp_model;
    vector<vector<vector<optional<BoolVar>>>> variables;
};
