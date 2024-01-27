#include "sat.h"
#include <format>

SAT::SAT(const Game &game) {
    variables.resize(game.get_components_count());
    for (int i = 0; i < game.get_components_count(); ++i) {
        variables[i].resize(game.get_components_count());
        for (int j = 0; j < game.get_components_count(); ++j) {
            variables[i][j].resize(game.get_player_count());
        }
    }
    vector<int> terminals = game.get_terminal_components();
    for (int i = 0; i < terminals.size(); ++i) {
        // Here we assume that an outcome can never be better than itself
        for (int k = 0; k < game.get_player_count(); ++k) {
            variables[terminals[i]][terminals[i]][k] = cp_model.FalseVar().WithName("I am always false");
        }
        for (int j = i + 1; j < terminals.size(); ++j) {
            for (int k = 0; k < game.get_player_count(); ++k) {
                variables[terminals[i]][terminals[j]][k] = cp_model.NewBoolVar().WithName(
                        format("X_{}_{}_{}", terminals[i], terminals[j], k));
            }
        }
    }
    // Add triangle restrictions here
    for (int player = 0; player < game.get_player_count(); ++player) {
        for (int first = 0; first < terminals.size(); ++first) {
            for (int second = 0; second < first; ++second) {
                for (int third = 0; third < second; ++third) {
                    int first_ter = terminals[first];
                    int second_ter = terminals[second];
                    int third_ter = terminals[third];
                    // a bit of a kostil here, i should have checked, that one of the edges is backwards,
                    // but we have triangles in both directions, so i can check, that one of them is in the right direction
                    vector<BoolVar> triangle;
                    triangle.push_back(get_var(first_ter, second_ter, player));
                    triangle.push_back(get_var(second_ter, third_ter, player));
                    triangle.push_back(get_var(third_ter, first_ter, player));
                    vector<BoolVar> backwards_triangle;
                    backwards_triangle.push_back(get_var(first_ter, second_ter, player).Not());
                    backwards_triangle.push_back(get_var(second_ter, third_ter, player).Not());
                    backwards_triangle.push_back(get_var(third_ter, first_ter, player).Not());
                    cp_model.AddBoolOr(triangle);
                    cp_model.AddBoolOr(backwards_triangle);
                }
            }
        }
    }
}

void SAT::solve() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE) {
        for (const auto &vec1: variables) {
            for (const auto &vec2: vec1) {
                for (optional<BoolVar> var: vec2) {
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

void SAT::print_results() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (!(response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE)) {
        cout << "Solution not found (((((((" << endl;
    }
    int k = variables[0][0].size();
    for (int player = 0; player < k; ++player) {
        vector<int> order;
        for (int i = 0; i < variables.size(); ++i) {
            bool found = false;
            int cnt_better = 0;
            for (const auto &vec2: variables[i]) {
                if (vec2[player]) {
                    found = true;
                    cnt_better += SolutionBooleanValue(response, vec2[player].value());
                }
            }
            if (!found) continue;
            order[cnt_better] = i;
        }
        cout << "Order for player " << player << ": ";
        for (auto elem: order) {
            cout << elem << ' ';
        }
        cout << '\n';
    }
}

BoolVar SAT::get_var(int i, int j, int k) {
    if (i < j) {
        return variables[i][j][k].value();
    } else {
        return variables[j][i][k].value().Not();
    }
}

void SAT::add_strategy(const Strategy &strat, const Game &game) {
    int outcome = game.play_strat(strat);
    vector<BoolVar> or_clause;
    for (int k = 0; k < game.get_player_count(); ++k) {
        for (const auto &neighbour_strat: game.neighbour_strategies(strat, k)) {
            int other_outcome = game.play_strat(neighbour_strat);
            if (other_outcome == outcome) continue;
            or_clause.push_back(get_var(other_outcome, outcome, k));
        }
    }
    cp_model.AddBoolOr(or_clause);
}

void SAT::add_all_strategies(const Game &game) {
    for (const auto &strat: game.generate_strategies()) {
        add_strategy(strat, game);
    }
}
