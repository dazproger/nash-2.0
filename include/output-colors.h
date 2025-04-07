#pragma once

#define RESET "\033[0m"
#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"

#define YELLOWLOG(message) std::cout << YELLOW << message << RESET << '\n';
#define REDLOG(message) std::cout << RED << message << RESET << '\n';

#define LOGSTART YELLOWLOG(std::string("[") + __func__ + "] START")
#define LOGEND YELLOWLOG(std::string("[") + __func__ + "] END\n")