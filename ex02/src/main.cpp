#include "../inc/PmergeMe.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error. Where are the numbers?\n";
        return 1;
    }
    try {
        PmergeMe pm;
        pm.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
