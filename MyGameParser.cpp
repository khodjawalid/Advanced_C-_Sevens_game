#include "MyGameParser.hpp"
#include <iostream>

namespace sevens {

void MyGameParser::read_game(const std::string& /*filename*/) {
    std::cout << "[MyGameParser] Initializing table layout with 7s...\n";

    table_layout.clear();

    for (uint64_t suit = 0; suit < 4; ++suit) {
        table_layout[suit][7] = true;
    }
}

} // namespace sevens

