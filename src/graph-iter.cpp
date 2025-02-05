#include "graph-iter.h"
#include "game.h"
#include "sat.h"
#include "string.h"
#include "nauty.h"
#include "gtools.h"
#include "checker.hpp"

#include <stdlib.h>

#define WORDSIZE 64

void generate_graph_nauty(int graph_size, const char* destination_file) 
{
    // generate shell command string and open pipe
    std::string command = "geng -c " + std::to_string(graph_size) + ' ' + destination_file;
    FILE* pipe = popen(command.c_str(), "r");

    // handle error
    if (!pipe) {
        std::cerr << "Failed to open pipe with command \"" << command.c_str() << "\"\n";
        exit(-1);
    }

    char buffer[128];
    std::cout << "Response" << std::endl;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        std::cout << buffer; // Выводим графы построчно

    // close pipe
    pclose(pipe);
}

void generate_directed_graph_nauty(const char* source_file, const char* destination_file) 
{
    // generate shell command and open pipe
    std::string command = std::string("directg ") + source_file + " " + destination_file;
    FILE* pipe = popen(command.c_str(), "r");

    // handle error
    if (!pipe) {
        std::cerr << "Failed to open pipe with command \"" << command.c_str() << "\"\n";
        exit(-1);    
    }

    char buffer[128];
    std::cout << "Response" << std::endl;

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        std::cout << buffer; // Выводим графы построчно

    // close pipe
    pclose(pipe);
}

bool dfs(std::vector<std::vector<int>>& graph, vector<int>& used, int v) {
    used[v] = 1;
    bool result = false;

    for (auto el : graph[v]) 
    {
        if (used[el] == 1)
            result = true;
        else if (used[el] == 0)
            result |= dfs(graph, used, el);
    }
    used[v] = 2;
    return result;
}

int graph_check(std::vector<std::vector<int>>& graph, vector<bool>& has_incoming_edges)
{
    // check for cycle
    vector<int> used(graph.size(), 0);
    bool has_cycle = false;

    for (auto i = 0LU; i < used.size(); ++i) {
        if (graph[i].size() == 0) // checking for dead-end vertices
            return -1;
        if (!used[i])
            has_cycle |= dfs(graph, used, i);
    }

    if (!has_cycle)
        return -1;
    else {
        //return 1; // delete later
    }
    
    // check for starting vertex, if found -> return it
    // can skip checking for cycles if we are checking for starting vertex
    vector<int> candidates;
    for (auto i = 0LU; i < has_incoming_edges.size(); ++i) 
    {
        if (!has_incoming_edges[i])
            candidates.push_back(i);
        used[i] = 0;
    }

    if (candidates.size() != 1)
        return -1;

    has_cycle = dfs(graph, used, candidates[0]);
    if (!has_cycle)
        return -1;

    for (auto element : used)
        if (!element)
            return -1;

    return candidates[0];
}

void filter_directg(const char* source_file)
{
    // open file
    FILE* file = fopen(source_file, "r");

    // handle error
    if (!file) {
        std::cerr << "Cannot open file " << source_file << std::endl;
        exit(-1);
    }

    int max_vertices = 64; // Максимальное количество вершин в графе
    const int m = (max_vertices + WORDSIZE - 1) / WORDSIZE; // Размер графа в словах
    graph g[max_vertices * m]; // Массив для хранения графа

    int num_graphs = 0;

    // Читаем графы из файла
    int n;
    int letters;
    int is_directed;
    while (readgg(file, g, 0, &letters, &n, &is_directed)) 
    {
        std::vector<std::vector<int>> graph_matrix(n);
        vector<bool> has_incoming_edges(n, false);

        for (int i = 0; i < n; ++i) 
        {
            for (int j = 0; j < n; ++j) 
            {
                if (ISELEMENT(GRAPHROW(g, i, letters), j)) 
                {
                    graph_matrix[i].push_back(j);
                    has_incoming_edges[j] = true;
                }
            }
        }

        int start = graph_check(graph_matrix, has_incoming_edges);
        if (start == -1)
            continue;

        // append terminals
        for (int i = 0; i <= n - 1; i++)
        {
            if (i == start)
                continue;
            graph_matrix.emplace_back(); // create new vertex (terminal)
            graph_matrix[i].push_back((int)graph_matrix.size() - 1); // link i-th vertex to the terminal
        }

        Game g(graph_matrix, start);
        g.set_graph_info();
        smart_check_skeleton(g, 0);
        if (num_graphs % 1000 == 0) {
            cout << num_graphs << "\n";
        }
        ++num_graphs;
    }
    std::cout << "found " << num_graphs << " with cycles (not necessarily with starting vertex)\n";
    fclose(file);
    return;
}

void graph_bruteforce(int n) {
    generate_graph_nauty(n, "nauty-graphs/from-geng.g6");
    generate_directed_graph_nauty("nauty-graphs/from-geng.g6", "nauty-graphs/from-directg.d6");
    filter_directg("nauty-graphs/from-directg.d6");
    //or add_start_directg();
    // parse to Game (in filter)
    // check (in filter)
}