#include "GreedyStrategy.hpp"
#include <algorithm>
#include <iostream>

namespace sevens {

void GreedyStrategy::initialize(uint64_t playerID) {
    myID = playerID;
    // No special initialization for this minimal version
}

int GreedyStrategy::selectCardToPlay(
    const std::vector<Card>& hand,
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout)
{
    int bestIndex = -1;
    int bestValue = -1;  // plus grand = mieux

    for (size_t i = 0; i < hand.size(); ++i) {
        const Card& card = hand[i];

        // Vérifie si la carte est jouable (adjacente à une carte déjà posée)
        auto it = tableLayout.find(card.suit);
        if (it != tableLayout.end()) {
            const auto& suitLayout = it->second;
            uint64_t value = card.rank;

            // Peut-on jouer cette carte ?
            bool isPlayable = false;
            if (value == 7) {
                isPlayable = true; // Les 7 sont toujours jouables
            } else if (value > 7) {
                isPlayable = suitLayout.count(value - 1); // carte précédente existe
            } else { // value < 7
                isPlayable = suitLayout.count(value + 1); // carte suivante existe
            }

            // Si jouable et de meilleure valeur, on la sélectionne
            if (isPlayable && static_cast<int>(value) > bestValue) {
                bestValue = value;
                bestIndex = static_cast<int>(i);
            }
        }
    }

    return bestIndex; // -1 = passe si aucune carte jouable
}


void GreedyStrategy::observeMove(uint64_t /*playerID*/, const Card& /*playedCard*/) {
    // Ignored in minimal version
}

void GreedyStrategy::observePass(uint64_t /*playerID*/) {
    // Ignored in minimal version
}

std::string GreedyStrategy::getName() const {
    return "GreedyStrategy";
}

extern "C" PlayerStrategy* createStrategy() {
    return new sevens::GreedyStrategy();
}

} // namespace sevens
