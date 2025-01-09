// Надо уметь перебирать все графы размера до n
// все графы с циклом
// Первая вершина зафиксирована из неё только исходять ребра
// потом к каждой вершине привязать терминал
// надо создавть инстанс класса graph, чтобы потом скармливать это в перебор

void generate_geng(int); 

void generate_directg();

// there should be a starting vertex
// every vertex is reachable from it and it has no incoming edges
void filter_directg(); // use readg from gtools.h or something else

// add starting vertex all possible ways
void add_start_directg(); // use readg from gtools.h or something else

void graph_bruteforce(int);