// Надо уметь перебирать все графы размера до n
// все графы с циклом
// Первая вершина зафиксирована из неё только исходять ребра
// потом к каждой вершине привязать терминал
// надо создавть инстанс класса graph, чтобы потом скармливать это в перебор

void generate_graph_nauty(int graph_size, const char* destination_file); 

void generate_directed_graph_nauty(const char* source_file, const char* destination_file);

// there should be a starting vertex
// every vertex is reachable from it and it has no incoming edges
 // use readg from gtools.h or something else
void filter_directg(const char* source_file);

// add starting vertex all possible ways
void filter_geng(const char* source_file, const char* out_file); // use readg from gtools.h or something else

void graph_bruteforce(int);