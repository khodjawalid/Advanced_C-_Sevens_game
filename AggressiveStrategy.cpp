#include "PlayerStrategy.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <random>
#include <chrono>

namespace sevens {

class AggressiveStrategy : public PlayerStrategy {
public:
    AggressiveStrategy() {
        auto seed = static_cast<unsigned long>(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        rng.seed(seed);
    }

    ~AggressiveStrategy() override = default;

    void initialize(uint64_t playerID) override {
        myID = playerID;
    }

    int selectCardToPlay(
    const std::vector<Card>& hand,
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout) override 
    {
        int bestIndex = -1;
        uint64_t bestRank = 0;
        bool foundPlayable = false;

        for (size_t i = 0; i < hand.size(); ++i) {
            const Card& card = hand[i];
            uint64_t suit = card.suit;
            uint64_t rank = card.rank;

            bool canPlay = false;

            auto itSuit = tableLayout.find(suit);

            if (rank == 7) {
                canPlay = true;
            } else if (itSuit != tableLayout.end()) {
                const auto& line = itSuit->second;
                if (rank > 1 && line.count(rank - 1)) {
                    canPlay = true;
                }
                if (rank < 13 && line.count(rank + 1)) {
                    canPlay = true;
                }
            }

            if (canPlay && (!foundPlayable || rank > bestRank)) {
                bestRank = rank;
                bestIndex = static_cast<int>(i);
                foundPlayable = true;
            }
        }

        return bestIndex; // -1 si aucune carte jouable
    }


    void observeMove(uint64_t playerID, const Card& playedCard) override {
        // Extension possible : suivi des cartes jouées
        (void)playerID;
        (void)playedCard;
    }

    void observePass(uint64_t playerID) override {
        // Extension possible : suivi des passes
        (void)playerID;
    }

    std::string getName() const override {
        return "AggressiveStrategy";
    }

private:
    uint64_t myID;
    std::mt19937 rng;
};

extern "C" PlayerStrategy* createStrategy() {
    return new AggressiveStrategy();
}

} // namespace sevens
