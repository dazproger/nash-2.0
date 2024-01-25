#include "strategy.h"
#include "game.h"
#include <ortools/sat/cp_model.h>

using namespace operations_research::sat;
using namespace std;

class SAT {
public:
    SAT(const Game&);
    void add_strategy(const Strategy&, const Game&);
    void solve();
private:
    CpModelBuilder cp_model;
};
