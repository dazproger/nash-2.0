#include "graph-iter.h"
#include "game.h"
#include "sat.h"
#include "cstring"
#include "nauty.h"
#include "gtools.h"
#include "checker.hpp"
#include <cstdlib>

#define WORDSIZE 64

#define RESET "\033[0m"
#define YELLOW "\033[1;33m"
#define YELLOWLOG(message) std::cout << YELLOW << message << RESET << '\n';
#define LOGSTART YELLOWLOG(std::string("[") + __func__ + "] START")
#define LOGEND YELLOWLOG(std::string("[") + __func__ + "] END\n")

void generate_graph_nauty(int graph_size, const char* destination_file) 
{
    LOGSTART

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

    LOGEND
}

void generate_directed_graph_nauty(const char* source_file, const char* destination_file) 
{
    LOGSTART

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

    LOGEND
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


// for oriented graphs
int graph_check(std::vector<std::vector<int>>& graph, vector<bool>& has_incoming_edges)
{
    vector<int> used(graph.size(), 0);
    bool has_cycle = false;
    
    // check for starting vertex, if found -> return it
    // also checking for cycle
    vector<int> candidates;
    for (auto i = 0LU; i < has_incoming_edges.size(); ++i) 
    {
        if (graph[i].empty()) // checking for dead-end vertices
            return -1;
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

    LOGSTART

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
        smart_check_skeleton(g, 1);
        if (num_graphs % 1000 == 0) {
            cout << num_graphs << "\n";
        }
        ++num_graphs;
    }
    std::cout << "found " << num_graphs << " with cycles (not necessarily with starting vertex)\n";
    fclose(file);

    LOGEND
}

void filter_geng(const char* source_file, const char* out_file) {
    // open file
    FILE* file = fopen(source_file, "r");
    FILE* outFile = fopen(out_file, "w");

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
        int amount_of_leaves = 0;

        for (int i = 0; i < n; ++i) 
        {   
            int degree = 0;
            for (int j = 0; j < n; ++j) 
            {
                if (ISELEMENT(GRAPHROW(g, i, letters), j)) 
                {
                    graph_matrix[i].push_back(j);
                    ++degree;
                }
            }
            if (degree == 1) {
                ++amount_of_leaves;
            }
        }
        if (amount_of_leaves <= 1) {
            writeg6(outFile, g, m, n);
        }
    }
    fclose(file);
    fclose(outFile);
}

void graph_bruteforce(int n) {
    generate_graph_nauty(n, "nauty-graphs/from-geng.g6");
    filter_geng("nauty-graphs/from-geng.g6", "nauty-graphs/filtered-geng.g6");
    generate_directed_graph_nauty("nauty-graphs/filtered-geng.g6", "nauty-graphs/from-directg.d6");
    filter_directg("nauty-graphs/from-directg.d6");
    //or add_start_directg();
    // parse to Game (in filter)
    // check (in filter)
}