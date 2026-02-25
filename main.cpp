#include <iostream>
#include <string>

int main() {
    std::string command;

    while (std::cin >> command) {
        if (command == "exit") {
            std::cout << "bye" << std::endl;
            break;
        } else if (command == "clean") {
            std::cout << "0" << std::endl;
        } else {
            // Placeholder for other commands
            // Skip the rest of the line
            std::string line;
            std::getline(std::cin, line);
            std::cout << "-1" << std::endl;
        }
    }

    return 0;
}
