#include "sat.h"
#include <iostream>

void SAT::solve() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    cout << CpSolverResponseStats(response) << endl;
}
