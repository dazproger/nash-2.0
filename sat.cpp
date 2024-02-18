#include "sat.h"
// #include <format>

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
                variables[terminals[i]][terminals[j]][k] =
                    cp_model.NewBoolVar();  //.WithName(format("X_{}_{}_{}", terminals[i], terminals[j], k));
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
                    // but we have triangles in both directions, so i can check, that one of them is in the right
                    // direction
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

void SAT::add_contraint(int i, int j, int k) {
    cp_model.AddBoolOr({get_var(i, j, k)});
}

bool SAT::is_solvable() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    return response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE;
}

void SAT::solve() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE) {
        for (const auto &vec1 : variables) {
            for (const auto &vec2 : vec1) {
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

static BoolVar get_var(int i, int j, int k, const VariableTable &variables) {
    if (i < j) {
        return variables[i][j][k].value();
    } else {
        return variables[j][i][k].value().Not();
    }
}

static void print_usual(const CpSolverResponse &response, const VariableTable &variables) {
    vector<int> terminals;
    for (int i = 0; i < variables.size(); ++i) {
        if (variables[i][i][0]) {
            terminals.push_back(i);
        }
    }
    int k = variables[0][0].size();
    for (int player = 0; player < k; ++player) {
        vector<int> order(terminals.size());
        for (int i : terminals) {
            int cnt_better = 0;
            for (int j : terminals) {
                if (i == j)
                    continue;
                cnt_better += SolutionBooleanValue(response, get_var(i, j, player, variables));
            }
            order[cnt_better] = i;
        }
        cout << "Order for player " << player << ": ";
        for (auto elem : order) {
            cout << elem << ' ';
        }
        cout << '\n';
    }
}

static void print_beautiful(const CpSolverResponse &response, const VariableTable &variables, const Game &game) {
    int n_comps = game.get_components_count();  // Number of components
    int n_verts = game.get_vertices_count();    // Number of vertexes
    auto component = game.get_components();     // colors of vertexes
    vector<vector<int>> components(n_comps);    // Vertices grouped by components

    for (int i = 0; i < n_verts; ++i) {
        components[component[i]].push_back(i);
    }
    vector<int> terminals = game.get_terminal_components();
    int k = game.get_player_count();
    vector<int> result(k, 0);
    for (int player = 0; player < k; ++player) {
        vector<int> order(terminals.size());
        for (int i = 0; i < terminals.size(); ++i) {
            int cnt_better = 0;
            int cnt_better_without_cycles = 0;
            for (int j = 0; j < terminals.size(); ++j) {
                if (i == j)
                    continue;
                cnt_better += SolutionBooleanValue(response, get_var(terminals[i], terminals[j], player, variables));
                if (components[terminals[j]].size() == 1) {
                    cnt_better_without_cycles +=
                        SolutionBooleanValue(response, get_var(terminals[i], terminals[j], player, variables));
                }
            }
            order[cnt_better] = i + 1;
            if (components[terminals[i]].size() > 1) {
                result[player] = max(cnt_better_without_cycles, result[player]);
            }
        }
        cout << "Order for player " << player + 1 << ": ";
        for (int i = 0; i < order.size(); ++i) {
            cout << (i ? " < " : "") << order[i];
        }
        cout << '\n';
    }
    std::sort(result.begin(), result.end(), greater<>());
    cout << "Result:\n";
    for (auto el : result) {
        cout << el << " ";
    }
    cout << '\n';
}

void SAT::print_results() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (!(response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE)) {
        cout << "Solution not found (((((((" << endl;
        return;
    }
    print_usual(response, variables);
}
void SAT::print_beautiful_results(const Game &game) {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (!(response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE)) {
        cout << "There always will be a Nash Equilibrium" << endl;
        return;
    }
    print_beautiful(response, variables, game);
}

void SAT::print_all_solutions() {
    Model model;
    int num_solutions = 0;
    model.Add(NewFeasibleSolutionObserver([&](const CpSolverResponse &response) {
        cout << "Solution #" << ++num_solutions << '\n';
        print_usual(response, variables);
    }));
    SatParameters parameters;
    parameters.set_enumerate_all_solutions(true);
    model.Add(NewSatParameters(parameters));
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
}
void SAT::print_all_beautiful_solutions(const Game &game) {
    game.print_terminal_descriptions();
    Model model;
    int num_solutions = 0;
    model.Add(NewFeasibleSolutionObserver([&](const CpSolverResponse &response) {
        cout << "Solution #" << ++num_solutions << '\n';
        print_beautiful(response, variables, game);
        cout << "\n////////////////////////////////////////////////////////////////////////////\n";
    }));
    SatParameters parameters;
    parameters.set_enumerate_all_solutions(true);
    model.Add(NewSatParameters(parameters));
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
}
BoolVar SAT::get_var(int i, int j, int k) {
    return ::get_var(i, j, k, variables);
}

void SAT::add_strategy(const Strategy &strat, const Game &game) {
    int outcome = game.play_strat(strat);
    vector<BoolVar> or_clause;
    for (int k = 0; k < game.get_player_count(); ++k) {
        for (const auto &neighbour_strat : game.neighbour_strategies(strat, k)) {
            int other_outcome = game.play_strat(neighbour_strat);
            if (other_outcome == outcome)
                continue;
            or_clause.push_back(get_var(other_outcome, outcome, k));
        }
    }
    cp_model.AddBoolOr(or_clause);
}

void SAT::add_all_strategies(const Game &game) {
    for (const auto &strat : game.generate_strategies()) {
        add_strategy(strat, game);
    }
}
