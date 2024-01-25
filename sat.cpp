#include "sat.h"
#include <iostream>

void SAT::solve() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE) {
        for (const auto& vec1 : variables) {
            for (const auto& vec2 : vec1) {
                for (BoolVar var : vec2) {
                    cout << var.Name() << " = " << SolutionBooleanValue(response, var) << '\n';
                }
                cout << '\n';
            }
            cout << "==================\n";
        }
    } else {
        cout << "Solution not found (((((((" << endl;
    }
}
