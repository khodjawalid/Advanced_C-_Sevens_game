#include "PlayerStrategy.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <string>

namespace sevens {

class RandomStrategy : public PlayerStrategy {
public:
    RandomStrategy() {
        auto seed = static_cast<unsigned long>(
            std::chrono::system_clock::now().time_since_epoch().count()
        );
        rng.seed(seed);
    }

    void initialize(uint64_t playerID) override {
        myID = playerID;
    }

    int selectCardToPlay(
        const std::vector<Card>& hand,
        const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout) override
    {
        std::vector<int> playableIndices;

        for (size_t i = 0; i < hand.size(); ++i) {
            const Card& card = hand[i];
            int suit = card.suit;
            int rank = card.rank;

            bool playable = false;

            if (rank == 7) {
                playable = true;
            } else {
                auto it = tableLayout.find(suit);
                if (it != tableLayout.end()) {
                    const auto& suitCards = it->second;
                    if (rank > 1 && suitCards.count(rank - 1)) {
                        playable = true;
                    }
                    if (rank < 13 && suitCards.count(rank + 1)) {
                        playable = true;
                    }
                }
            }

            if (playable) {
                playableIndices.push_back(static_cast<int>(i));
            }
        }

        if (playableIndices.empty()) {
            return -1; // Passe si aucune carte jouable
        }

        std::uniform_int_distribution<int> dist(0, static_cast<int>(playableIndices.size()) - 1);
        return playableIndices[dist(rng)];
    }

    void observeMove(uint64_t, const Card&) override {}
    void observePass(uint64_t) override {}

    std::string getName() const override {
        return "RandomStrategy";
    }

private:
    uint64_t myID;
    std::mt19937 rng;
};

extern "C" PlayerStrategy* createStrategy() {
    return new RandomStrategy();
}

} // namespace sevens
