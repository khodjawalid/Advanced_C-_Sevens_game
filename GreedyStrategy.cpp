#include "PlayerStrategy.hpp"
#include <vector>
#include <string>
#include <algorithm>

namespace sevens {

class GreedyStrategy : public PlayerStrategy {
public:
    void initialize(uint64_t playerID) override {
        myID = playerID;
    }

    int selectCardToPlay(
        const std::vector<Card>& hand,
        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout) override
    {
        int bestIndex = -1;
        int bestRank = -1;

        for (size_t i = 0; i < hand.size(); ++i) {
            const Card& card = hand[i];
            int suit = card.suit;
            int rank = card.rank;

            bool playable = false;

            if (rank == 7) {
                playable = true;
            } else {
                auto itSuit = tableLayout.find(suit);
                if (itSuit != tableLayout.end()) {
                    const auto& suitCards = itSuit->second;
                    if (rank > 1 && suitCards.count(rank - 1)) {
                        playable = true;
                    }
                    if (rank < 13 && suitCards.count(rank + 1)) {
                        playable = true;
                    }
                }
            }

            if (playable && rank > bestRank) {
                bestRank = rank;
                bestIndex = static_cast<int>(i);
            }
        }

        return bestIndex; // -1 si aucune carte jouable
    }

    void observeMove(uint64_t, const Card&) override {}
    void observePass(uint64_t) override {}

    std::string getName() const override {
        return "GreedyStrategy";
    }

private:
    uint64_t myID;
};

extern "C" PlayerStrategy* createStrategy() {
    return new GreedyStrategy();
}

} // namespace sevens
