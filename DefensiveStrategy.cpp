
#include "DefensiveStrategy.hpp"
#include <algorithm>
#include <chrono>

namespace sevens {

DefensiveStrategy::DefensiveStrategy() {
    auto seed = static_cast<unsigned long>(
        std::chrono::system_clock::now().time_since_epoch().count()
    );
    rng.seed(seed);
}

void DefensiveStrategy::initialize(uint64_t playerID) {
    myID = playerID;
}

int DefensiveStrategy::selectCardToPlay(
    const std::vector<Card>& hand,
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout)
{
    std::vector<int> playableIndices;

    for (size_t i = 0; i < hand.size(); ++i) {
        const Card& card = hand[i];
        int suit = card.suit;
        int rank = card.rank;

        bool playable = false;
        auto it = tableLayout.find(suit);
        if (rank == 7) {
            playable = true;
        } else if (it != tableLayout.end()) {
            const auto& suitCards = it->second;
            if ((rank > 1 && suitCards.count(rank - 1)) ||
                (rank < 13 && suitCards.count(rank + 1))) {
                playable = true;
            }
        }

        if (playable) {
            playableIndices.push_back(static_cast<int>(i));
        }
    }

    if (playableIndices.empty()) return -1;

    // Joue la carte la moins dangereuse (rang le plus petit)
    auto cmp = [&](int a, int b) {
        return hand[a].rank < hand[b].rank;
    };
    return *std::min_element(playableIndices.begin(), playableIndices.end(), cmp);
}

void DefensiveStrategy::observeMove(uint64_t, const Card&) {}
void DefensiveStrategy::observePass(uint64_t) {}

std::string DefensiveStrategy::getName() const {
    return "DefensiveStrategy";
}

extern "C" PlayerStrategy* createStrategy() {
    return new DefensiveStrategy();
}

} // namespace sevens
