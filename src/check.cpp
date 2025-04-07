#include <fstream>
#include <vector>
#include <iostream>
#include "checker.hpp"

int main() 
{
    Checker checker;
    if (checker.check()) {
        std::cout << "OK.\n";
    } else {
        std::cout << "Something went wrong.\n";
    }
}
