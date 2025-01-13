#include "graph-iter.h"
#include "game.h"
#include "sat.h"
#include "string.h"
#include "nauty.h"
#include "gtools.h"
#define WORDSIZE 64


void generate_geng(int n) {
    std::string command = ("geng -c " + std::to_string(n) + " nauty-graphs/from-geng.g6"); // Команда для генерации графов
    char shell_path[command.length() + 1];
    std::strcpy(shell_path, command.c_str());
    FILE* pipe = popen(shell_path, "r");
    if (!pipe) {
        std::cerr << "Ошибка: не удалось выполнить команду geng!" << std::endl;
        return;
    }

    char buffer[128];
    std::cout << "Response" << std::endl;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {

        std::cout << buffer; // Выводим графы построчно
    }
    pclose(pipe);
}

void generate_directg() {
    const char* shell_path = "directg nauty-graphs/from-geng.g6 nauty-graphs/from-directg.d6";
    FILE* pipe = popen(shell_path, "r");
    if (!pipe) {
        std::cerr << "Ошибка: не удалось выполнить команду directg!" << std::endl;
        return;
    }

    char buffer[128];
    std::cout << "Response" << std::endl;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {

        std::cout << buffer; // Выводим графы построчно
    }
    pclose(pipe);
}

bool dfs(std::vector<std::vector<int>>& graph, vector<int>& used, int v) {
    used[v] = 1;
    bool flag = false;
    for (auto el : graph[v]) {
        if (used[el] == 1) {
            flag = true;
        }
        else if (used[el] == 0) {
            flag |= dfs(graph, used, el);
        }
    }
    used[v] = 2;
    return flag;
}
int graph_check(std::vector<std::vector<int>>& graph, vector<int>& has_incoming_edges){
    // check for cycle
    vector<int> used(graph.size(), 0);
    bool has_cycle = false;
    for (int i = 0; i < used.size();++i) {
        if (!used[i]) {
            has_cycle |= dfs(graph, used, i);
        }
    }
    if (!has_cycle) {
        return -1;
    } else {
        //return 1; // delete later
    }
    
    // check for starting vertex, if found -> return it
    // can skip checking for cycles if we are checking for starting vertex
    vector<int> candidates;
    for (int i = 0; i < has_incoming_edges.size();++i) {
        if (!has_incoming_edges[i]) {
            candidates.push_back(i);
        }
        used[i] = 0;
    }
    if (candidates.size() != 1) {
        return -1;
    }

    has_cycle = dfs(graph, used, candidates[0]);
    if (!has_cycle) {
        return -1;
    }
    for (auto el : used) {
        if (!el) {
            return -1;
        }
    }
    return candidates[0];
}

void filter_directg(){
    const char* filename = "nauty-graphs/from-directg.d6"; // Имя файла с графами в формате graph6
    FILE* file = fopen(filename, "r");
    if (!file) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return;
    }

    int max_vertices = 64; // Максимальное количество вершин в графе
    const int m = (max_vertices + WORDSIZE - 1) / WORDSIZE; // Размер графа в словах
    graph g[max_vertices * m]; // Массив для хранения графа

    int num_graphs = 0;

    // Читаем графы из файла
    int n;
    int letters;
    int is_directed;
    while (readgg(file, g, 0, &letters, &n, &is_directed)) {
        // Выводим матрицу смежности
        std::vector<std::vector<int>> graph(n);
        vector<int> has_incoming_edges(n, 0);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (ISELEMENT(GRAPHROW(g, i, letters), j)) {
                    graph[i].push_back(j);
                    has_incoming_edges[j] = 1;
                }
            }
        }

        int start;
        if ((start = graph_check(graph, has_incoming_edges)) != -1) {
            // TODO: 
            ++num_graphs;
        }


    }
    std::cout << "found " << num_graphs << " with cycles   (not necessarily with starting vertex)  it depends on 74 line\n";
    fclose(file);
    return;
}

void graph_bruteforce(int n) {
    generate_geng(n);
    generate_directg();
    filter_directg();
    //or add_start_directg();
    // parse to Game (in filter)
    // check (in filter)
}