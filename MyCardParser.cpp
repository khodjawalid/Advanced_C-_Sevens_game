#include "MyCardParser.hpp"
#include <iostream>

namespace sevens {

void MyCardParser::read_cards(const std::string& /*filename*/) {
    std::cout << "[MyCardParser] Generating standard 52-card deck...\n";

    cards_hashmap.clear();
    uint64_t id = 0;

    for (uint64_t suit = 0; suit < 4; ++suit) {
        for (uint64_t rank = 1; rank <= 13; ++rank) {
            cards_hashmap[id++] = Card(static_cast<int>(suit), static_cast<int>(rank));
        }
    }
}

} // namespace sevens

