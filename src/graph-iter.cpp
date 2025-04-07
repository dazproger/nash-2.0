#include "graph-iter.h"
#include "game.h"
#include "sat.h"
#include "cstring"
#include "nauty.h"
#include "gtools.h"
#include "checker.hpp"
#include "output-colors.h"

#include <cstdlib>
#include <filesystem>

#define WORDSIZE 64

void generate_graph_nauty(int graph_size, const char* destination_file) 
{
    LOGSTART

    // Shell command string generation and pipe opening
    const std::string command = "geng -c " + std::to_string(graph_size) + ' ' + destination_file;
    FILE* pipe = popen(command.c_str(), "r");

    // Error handling
    if (!pipe) {
        std::cerr << "Failed to open pipe with command \"" << command.c_str() << "\"\n";
        exit(-1);
    }

    char buffer[128];
    std::cout << "Response" << std::endl;

    // Line-by-line graph output
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        std::cout << buffer;

    // Pipe closing
    pclose(pipe);

    LOGEND
}

void generate_directed_graph_nauty(const char* source_file, const char* destination_file) 
{
    LOGSTART

    // Shell command string generation and pipe opening
    std::string command = std::string("directg \'") + source_file + "\' \'" + destination_file + "\'";
    FILE* pipe = popen(command.c_str(), "r");

    // Error handling
    if (!pipe) {
        std::cerr << "Failed to open pipe with command \"" << command.c_str() << "\"\n";
        exit(-1);    
    }

    char buffer[128];
    std::cout << "Response" << std::endl;

    // Line-by-line graph output
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        std::cout << buffer;

    // Pipe closing
    pclose(pipe);

    LOGEND
}

void generate_directed_graph_o_nauty(const char* source_file, const char* destination_file) 
{
    LOGSTART

    // Shell command string generation and pipe opening
    std::string command = std::string("directg -o \'") + source_file + "\' \'" + destination_file + "\'";
    FILE* pipe = popen(command.c_str(), "r");

    // Error handling
    if (!pipe) {
        std::cerr << "Failed to open pipe with command \"" << command.c_str() << "\"\n";
        exit(-1);    
    }

    char buffer[128];
    std::cout << "Response" << std::endl;

    // Line-by-line graph output
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        std::cout << buffer;

    // Pipe closing
    pclose(pipe);

    LOGEND
}

bool dfs(std::vector<std::vector<int>>& graph, vector<int>& used, int v) {
    used[v] = 1;
    bool result = false;

    for (const auto element : graph[v])
    {
        if (used[element] == 1)
            result = true;
        else if (used[element] == 0)
            result |= dfs(graph, used, element);
    }

    used[v] = 2;
    return result;
}

bool has_no_cycles(const std::vector<std::vector<int>>& graph) 
{
    int n = graph.size();
    std::vector<bool> visited(n, false);
    std::vector<bool> rec_stack(n, false);

    auto lambda_dfs = 
    [&](int node, auto&& lambda_dfs) -> bool 
    {
        visited[node] = true;
        rec_stack[node] = true;

        for (int neighbor : graph[node]) {
            if (!visited[neighbor])
                if (lambda_dfs(neighbor, lambda_dfs))
                    return true;
            else if (rec_stack[neighbor])
                return true;
        }

        rec_stack[node] = false;
        return false;
    };

    for (int node = 0; node < n; ++node) {
        if (!visited[node]) {
            if (lambda_dfs(node, lambda_dfs)) {
                return false;
            }
        }
    }

    return true;
}

// For oriented graphs
int graph_check(std::vector<std::vector<int>>& graph, vector<bool>& has_incoming_edges)
{
    vector<int> used(graph.size(), 0);
    bool has_cycle = false;
    
    // Check for starting vertex, if found -> return it
    // Also checking for cycle
    vector<int> candidates;
    for (auto i = 0LU; i < has_incoming_edges.size(); ++i) 
    {
        if (graph[i].empty()) // Checking for dead-end vertices
            return -1;
        if (!has_incoming_edges[i])
            candidates.push_back((int)i);
        used[i] = 0;
    }

    if (candidates.size() != 1)
        return -1;

    has_cycle = dfs(graph, used, candidates[0]);
    if (!has_cycle)
        return -1;

    for (const auto element : used)
        if (!element)
            return -1;


    return candidates[0];
}

void filter_directg(const char* source_file)
{
    LOGSTART

    // File opening
    FILE* file = fopen(source_file, "r");

    // Error handling
    if (!file) {
        std::cerr << "Cannot open file " << source_file << std::endl;
        exit(-1);
    }

    const int max_vertices = 64;
    const int m = (max_vertices + WORDSIZE - 1) / WORDSIZE; // Graph size in words
    graph g[max_vertices * m];

    int num_graphs = 0;

    // Reading graphs from files
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

        // Terminals appending
        for (int i = 0; i <= n - 1; i++)
        {
            if (i == start)
                continue;
            graph_matrix.emplace_back();
            graph_matrix[i].push_back((int)graph_matrix.size() - 1);
        }

        Game g(graph_matrix, start);
        g.set_graph_info();
        smart_check_skeleton(g, 1);
        if (num_graphs % 1000 == 0) {
            std::cout << num_graphs << "\n";
        }
        ++num_graphs;
    }
    std::cout << "found " << num_graphs << " with cycles (not necessarily with starting vertex)\n";
    fclose(file);

    LOGEND
}

void filter_DAGs(const char* source_file, const char* destination_file)
{
    // File opening
    FILE* file = fopen(source_file, "r");
    FILE* outFile = fopen(destination_file, "w");

    // Error handling
    if (!file) {
        std::cerr << "Cannot open file " << source_file << std::endl;
        exit(-1);
    }

    if (!outFile) {
        std::cerr << "Cannot open file " << destination_file << std::endl;
        exit(-1);
    }

    const int max_vertices = 64;
    const int m = (max_vertices + WORDSIZE - 1) / WORDSIZE; // Graph size in words
    graph g[max_vertices * m];

    int num_graphs = 0;

    // Reading graphs from files
    int n;
    int letters;
    int is_directed;
    while (readgg(file, g, 0, &letters, &n, &is_directed)) 
    {
        std::vector<std::vector<int>> graph_matrix(n);
        int amount_of_leaves = 0;

        for (int i = 0; i < n; ++i) 
            for (int j = 0; j < n; ++j) 
                if (ISELEMENT(GRAPHROW(g, i, letters), j)) 
                    graph_matrix[i].push_back(j);

        if (has_no_cycles(graph_matrix))
            writed6(outFile, g, m, n);
    }
    fclose(file);
    fclose(outFile);
}

void filter_geng(const char* source_file, const char* out_file) {
    // File opening
    FILE* file = fopen(source_file, "r");
    FILE* outFile = fopen(out_file, "w");

    // Error handling
    if (!file) {
        std::cerr << "Cannot open file " << source_file << std::endl;
        exit(-1);
    }

    const int max_vertices = 64;
    const int m = (max_vertices + WORDSIZE - 1) / WORDSIZE; // Graph size in words
    graph g[max_vertices * m];

    int num_graphs = 0;

    // Reading graphs from files
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

const char generated[] = "nauty-graphs/from-geng.g6";
const char filtered[] = "nauty-graphs/filtered-geng.g6";
const char oriented[] = "nauty-graphs/from-directg.d6";
const char filtered_acyclic[] = "nauty-graphs/filtered-pickg.g6";


void graph_bruteforce(int n) 
{
    std::filesystem::create_directory("nauty-graphs");

    generate_graph_nauty(n, generated);
    filter_geng(generated, filtered);
    generate_directed_graph_o_nauty(filtered, oriented);
    filter_directg(oriented);
}
