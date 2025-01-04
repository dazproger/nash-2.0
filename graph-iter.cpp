#include "graph-iter.h"
#include "game.h"
#include "sat.h"
#include "string.h"


void generate_geng(int n) {
    std::string command = ("geng " + std::to_string(n) + " nauty-graphs/from-geng.txt"); // Команда для генерации графов
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
    const char* geng_command2 = "directg nauty-graphs/from-geng.txt nauty-graphs/from-directg.txt"; // Команда для генерации графов
    pclose(pipe);
}

void generate_directg() {
    const char* shell_path = "directg nauty-graphs/from-geng.txt nauty-graphs/from-directg.txt";
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

void graph_bruteforce(int n) {
    generate_geng(n);
    generate_directg();
    // filter_directg() or add_start_directg();
    // parse to Game
    // check
}