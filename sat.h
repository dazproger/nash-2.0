#include "strategy.h"
#include "game.h"
#include <ortools/sat/cp_model.h>

using namespace operations_research::sat;
using namespace std;

typedef vector<vector<vector<optional<BoolVar>>>> VariableTable;

class SAT {
public:
    // Creates SAT by Game. After this constructor SAT should already have all X_ijk variables created and all triangle restrictions should be added.
    // Don't forget to name the variables
    SAT(const Game&);
    // Adds strategy restriction to the SAT
    void add_strategy(const Strategy&, const Game&);
    // Adds all strategies from the game
    void add_all_strategies(const Game&);
    // Adds a constraint that an outcome is better than another outcome for a fixed player
    void add_constraint(int, int, int);
    // Add constraints that specify that a loop should not have a rank > 1
    void minimize_loop_rank(int, int);
    // Add constraints on all loops other from this to have rank <= 1
    void minimize_all_except(int, int, const Game&);
    // Return an array of orders for all players, or an empty array if none was found
    vector<vector<int>> return_results(const Game&); // TODO
    // Checks if the SAT is solvable
    bool is_solvable();
    // Solves the SAT and prints the results
    void solve();
    // Should add another function that extracts data from solved SAT
    void print_results();
    /// Print any solution in human-readable way
    void print_beautiful_results(const Game&);
    // Prints all solutions of SAT
    void print_all_solutions();
    /// Print all solution in human-readable way
    void print_all_beautiful_solutions(const Game&);
    // Copy of print_all_beautiful_solutions, but prints only those permutations which are close to solving our problem
    void print_all_solutions_close_to_c22(const Game&); // TODO: create and fix print_all_beautiful_solutions
    // Returns the X_ijk Boolean variable
    BoolVar get_var(int i, int j, int k);
private:
    CpModelBuilder cp_model;
    VariableTable variables;
};
