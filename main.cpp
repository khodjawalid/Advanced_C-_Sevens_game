#include <iostream>
#include <string>
#include <vector>
#include "MyGameMapper.hpp"
#include "StrategyLoader.hpp"

using namespace sevens;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./sevens_game competition strategy1.so strategy2.so ...\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "competition") {
        std::vector<std::string> strategyFiles;
        for (int i = 2; i < argc; ++i) {
            strategyFiles.push_back(argv[i]);
        }

        std::vector<std::shared_ptr<PlayerStrategy>> strategies;
        for (const auto& file : strategyFiles) {
            auto strategy = StrategyLoader::loadFromLibrary(file);
            if (strategy) {
                strategies.push_back(std::move(strategy));
                std::cout << "Loaded strategy: " << strategies.back()->getName() << std::endl;
            } else {
                std::cerr << "Failed to load strategy from: " << file << std::endl;
                return 1;
            }
        }

        MyGameMapper game;
        game.runCompetition(strategies);
    } else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        return 1;
    }

    return 0;
}
