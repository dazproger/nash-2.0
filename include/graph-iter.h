void generate_graph_nauty(int graph_size, const char* destination_file);

void generate_directed_graph_nauty(const char* source_file, const char* destination_file);

// There should be a starting vertex
// Every vertex is reachable from it, and it has no incoming edges
// use readg from gtools.h or something else
void filter_directg(const char* source_file);

// Add starting vertex all possible ways
void filter_geng(const char* source_file, const char* out_file); // Use readg from gtools.h or something else

void graph_bruteforce(int);