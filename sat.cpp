#include "sat.h"
#include <iostream>

void SAT::solve() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE) {
        for (const auto& vec1 : variables) {
            for (const auto& vec2 : vec1) {
                for (optional<BoolVar> var : vec2) {
                    if (var) {
                        cout << var.value().Name() << " = " << SolutionBooleanValue(response, var.value()) << '\n';
                    }
                }
                cout << '\n';
            }
            cout << "==================\n";
        }
    } else {
        cout << "Solution not found (((((((" << endl;
    }
}

BoolVar SAT::get_var(int i, int j, int k) {
    if (i < j) {
        return variables[i][j][k].value();
    } else {
        return variables[j][i][k].value().Not();
    }
}

void SAT::add_strategy(const Strategy& strat, const Game& game) {
    int outcome = game.play_strat(strat);
    for (int k = 0; k < game.get_player_count(); ++k) {
        vector<BoolVar> or_clause;
        for (const auto& neighbour_strat : game.neighbour_strategies(strat, k)) {
            int other_outcome = game.play_strat(neighbour_strat);
            if (other_outcome == outcome) continue;
            or_clause.push_back(get_var(other_outcome, outcome, k));
        }
        cp_model.AddBoolOr(or_clause);
    }
}

void SAT::add_all_strategies(const Game& game) {
    for (const auto& strat : game.generate_strategies()) {
        add_strategy(strat, game);
    }
}
