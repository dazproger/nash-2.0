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
    vector<int> cnt_component = game.get_cnt_components();
    is_cycle.resize(cnt_component.size());
    for (int i = 0; i < cnt_component.size(); ++i) {
        is_cycle[i] = cnt_component[i] > 1;
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

void SAT::add_constraint(int i, int j, int k) {
    cp_model.AddBoolOr({get_var(i, j, k)});
}

static vector<int> get_terminals(const VariableTable &variables) {
    vector<int> terminals;
    for (int i = 0; i < variables.size(); ++i) {
        if (variables[i][i][0]) {
            terminals.push_back(i);
        }
    }
    return terminals;
}

void SAT::minimize_loop_rank(int cycle, int player) {
    if (!is_cycle[cycle]) {
        cerr << "\x1b[31;1m";  // Print in bold red
        cerr << "NOT A CYCLE" << endl;
        cerr << "\x1b[0m";  // Reset color
        return;
    }
    vector<int> terminals = get_terminals(variables);
    vector<BoolVar> clause;
    for (auto terminal : terminals) {
        if (is_cycle[terminal])
            continue;
        clause.push_back(get_var(cycle, terminal, player));
    }
    cp_model.AddAtMostOne(clause);
}

void SAT::minimize_all_except(int good_cycle, int good_player) {
    vector<int> terminals = get_terminals(variables);
    int player_count = variables[0][0].size();
    for (auto terminal : terminals) {
        if (!is_cycle[terminal])
            continue;
        for (int player = 0; player < player_count; ++player) {
            if (player == good_player && terminal == good_cycle)
                continue;
            minimize_loop_rank(terminal, player);
        }
    }
}

void SAT::limit_one_loop_rank(int cycle, int player, int rank) {
    vector<int> terminals = get_terminals(variables);
    vector<int> non_cycles;
    for (const auto &terminal : terminals) {
        if (!is_cycle[terminal]) {
            non_cycles.push_back(terminal);
        }
    }
    if (rank >= non_cycles.size())
        return;
    vector<int> mask(non_cycles.size(), 0);
    ++rank;
    for (int i = 0; i < rank; ++i) {
        mask[mask.size() - 1 - i] = 1;
    }
    do {
        vector<BoolVar> clause;
        for (int i = 0; i < mask.size(); ++i) {
            if (mask[i]) {
                clause.push_back(get_var(non_cycles[i], cycle, player));
            }
        }
        cp_model.AddBoolOr(clause);
    } while (next_permutation(mask.begin(), mask.end()));
}

void SAT::limit_many_loop_ranks(vector<int> ranks) {
    int player_cnt = variables[0][0].size();
    vector<int> cycles;
    for (const auto &terminal : get_terminals(variables)) {
        if (is_cycle[terminal]) {
            cycles.push_back(terminal);
        }
    }
    assert(cycles.size() * player_cnt == ranks.size());
    int cur_rank = 0;
    for (int i = 0; i < player_cnt; ++i) {
        for (auto cycle : cycles) {
            limit_one_loop_rank(cycle, i, ranks[cur_rank++]);
        }
    }
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
    vector<int> terminals = get_terminals(variables);
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

static vector<int> get_ranks(const CpSolverResponse &response, const VariableTable &variables,
                             const vector<bool> &is_cycle) {
    vector<int> terminals = get_terminals(variables);
    int k = variables[0][0].size();
    vector<int> result;
    for (int player = 0; player < k; ++player) {
        for (int i = 0; i < terminals.size(); ++i) {
            int cnt_better_without_cycles = 0;
            for (int j = 0; j < terminals.size(); ++j) {
                if (i == j)
                    continue;
                if (!is_cycle[terminals[j]]) {
                    cnt_better_without_cycles +=
                        SolutionBooleanValue(response, get_var(terminals[i], terminals[j], player, variables));
                }
            }
            if (is_cycle[terminals[i]]) {
                result.push_back(cnt_better_without_cycles);
            }
        }
    }
    std::sort(result.begin(), result.end(), greater<>());
    return result;
}

static void print_beautiful(const CpSolverResponse &response, const VariableTable &variables, vector<bool> is_cycle) {
    vector<int> terminals = get_terminals(variables);
    int k = variables[0][0].size();
    vector<int> result;
    for (int player = 0; player < k; ++player) {
        vector<int> order(terminals.size());
        for (int i = 0; i < terminals.size(); ++i) {
            int cnt_better = 0;
            int cnt_better_without_cycles = 0;
            for (int j = 0; j < terminals.size(); ++j) {
                if (i == j)
                    continue;
                cnt_better += SolutionBooleanValue(response, get_var(terminals[i], terminals[j], player, variables));
                if (!is_cycle[terminals[j]]) {
                    cnt_better_without_cycles +=
                        SolutionBooleanValue(response, get_var(terminals[i], terminals[j], player, variables));
                }
            }
            order[cnt_better] = i + 1;
            if (is_cycle[terminals[i]]) {
                result.push_back(cnt_better_without_cycles);
            }
        }
        cout << "Order for player " << player + 1 << ": ";
        for (int i = 0; i < order.size(); ++i) {
            cout << (i ? " < " : "");
            if (is_cycle[terminals[order[i] - 1]])
                cout << "\x1b[32;1m";
            cout << order[i];
            if (is_cycle[terminals[order[i] - 1]])
                cout << "\x1b[0m";
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
void SAT::print_beautiful_results() {
    Model model;
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
    if (!(response.status() == CpSolverStatus::OPTIMAL || response.status() == CpSolverStatus::FEASIBLE)) {
        cout << "There always will be a Nash Equilibrium" << endl;
        return;
    }
    print_beautiful(response, variables, is_cycle);
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
void SAT::print_all_beautiful_solutions() {
    Model model;
    int num_solutions = 0;
    model.Add(NewFeasibleSolutionObserver([&](const CpSolverResponse &response) {
        cout << "Solution #" << ++num_solutions << '\n';
        print_beautiful(response, variables, is_cycle);
        cout << "\n////////////////////////////////////////////////////////////////////////////\n";
    }));
    SatParameters parameters;
    parameters.set_enumerate_all_solutions(true);
    model.Add(NewSatParameters(parameters));
    const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);
}

void SAT::print_all_solutions_close_to_c22() {
    Model model;
    int num_solutions = 0;
    model.Add(NewFeasibleSolutionObserver([&](const CpSolverResponse &response) {
        vector<int> ranks = get_ranks(response, variables, is_cycle);
        if (ranks[1] > 2) {
            return;
        }
        cout << "Solution #" << ++num_solutions << '\n';
        print_beautiful(response, variables, is_cycle);
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
        //for (const auto &neighbour_strat : game.neighbour_strategies(strat, k)) {
        //    int other_outcome = game.play_strat(neighbour_strat);
        //    if (other_outcome == outcome)
        //        continue;
        //    or_clause.push_back(get_var(other_outcome, outcome, k));
        //}
        for (auto other_outcome : game.neighbour_strategies_outcomes(strat, k)) {
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

bool try_achieve_ranks(vector<int> ranks, const Game &g) {
    int cnt_cycles = g.get_cycles().size();
    for (size_t i = ranks.size(); i < g.get_player_count() * cnt_cycles; ++i) {
        ranks.push_back(ranks.back());
    }
    sort(ranks.begin(), ranks.end());
    do {
        SAT s(g);
        s.add_all_strategies(g);
        s.limit_many_loop_ranks(ranks);
        if (s.is_solvable()) {
            return true;
        }
    } while (next_permutation(ranks.begin(), ranks.end()));
    return false;
}

void print_example_achieve_ranks(vector<int> ranks, const Game &g) {
    int cnt_cycles = g.get_cycles().size();
    for (size_t i = ranks.size(); i < cnt_cycles * g.get_player_count(); ++i) {
        ranks.push_back(ranks.back());
    }
    sort(ranks.begin(), ranks.end());
    do {
        SAT s(g);
        s.add_all_strategies(g);
        s.limit_many_loop_ranks(ranks);
        if (s.is_solvable()) {
            s.print_beautiful_results();
            return;
        }
    } while (next_permutation(ranks.begin(), ranks.end()));
    cout << "No res :sad_face:" << endl;
}

void print_all_achieve_ranks(vector<int> ranks, const Game &g) {
    int cnt_cycles = g.get_cycles().size();
    for (size_t i = ranks.size(); i < cnt_cycles * g.get_player_count(); ++i) {
        ranks.push_back(ranks.back());
    }
    sort(ranks.begin(), ranks.end());
    do {
        SAT s(g);
        s.add_all_strategies(g);
        s.limit_many_loop_ranks(ranks);
        if (s.is_solvable()) {
            s.print_all_beautiful_solutions();
        }
    } while (next_permutation(ranks.begin(), ranks.end()));
}

void print_all_solutions_close_to_c22(const Game &g) {
    vector<int> ranks(g.get_cycles().size() * g.get_player_count(), 2);
    //ranks[0] = static_cast<int>(1e9) + 10;
    print_all_achieve_ranks(ranks, g);
}
