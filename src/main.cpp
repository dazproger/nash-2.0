#include <iostream>
#include "graph-iter.h"
#include "output-colors.h"

int main(int argc, __attribute__((unused)) char* argv[]) 
{
    (void)argc;

    int n;
    REDLOG("Do not enter numbers higher than 6!");
    std::cin >> n;

    graph_bruteforce(n);
}