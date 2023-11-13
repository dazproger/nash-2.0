#include <ortools/sat/cp_model.h>

using namespace operations_research::sat;
using namespace std;

int main() {
    CpModelBuilder cp_model;
    const BoolVar a = cp_model.NewBoolVar().WithName("a");
    const BoolVar b = cp_model.NewBoolVar();
    const BoolVar c = cp_model.NewBoolVar();
    cp_model.AddBoolOr({a, b});
    cp_model.AddBoolOr({Not(a), Not(b), Not(c)});
    cp_model.AddBoolOr({c});
    // cp_model.AddBoolOr({Not(c)});
    int num_solutions = 0;
    Model model;
    model.Add(NewFeasibleSolutionObserver([&](const CpSolverResponse& r) {
        cout << "Solution " << num_solutions;
        cout << "  a = " << SolutionBooleanValue(r, a);
        cout << "  b = " << SolutionBooleanValue(r, b);
        cout << "  c = " << SolutionBooleanValue(r, c) << endl;
    num_solutions++;
    }));
    // Solving part.
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    cout << CpSolverResponseStats(response);
    return 0;
}
