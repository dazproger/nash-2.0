#include <fstream>
#include <vector>
#include <iostream>
#include "checker.hpp"

using std::cout;
using std::vector;

int main() 
{
    Checker checker;
    if (checker.check()) {
        cout << "OK.\n";
    } else {
        cout << "Something went wrong.\n";
    }
}
