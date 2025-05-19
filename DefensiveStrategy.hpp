
#ifndef DEFENSIVE_STRATEGY_HPP
#define DEFENSIVE_STRATEGY_HPP

#include "PlayerStrategy.hpp"
#include <random>

namespace sevens {

class DefensiveStrategy : public PlayerStrategy {
public:
    DefensiveStrategy();
    void initialize(uint64_t playerID) override;
    int selectCardToPlay(const std::vector<Card>& hand,
                         const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& tableLayout) override;
    void observeMove(uint64_t playerID, const Card& playedCard) override;
    void observePass(uint64_t playerID) override;
    std::string getName() const override;

private:
    uint64_t myID;
    std::mt19937 rng;
};

extern "C" PlayerStrategy* createStrategy();

} // namespace sevens

#endif
