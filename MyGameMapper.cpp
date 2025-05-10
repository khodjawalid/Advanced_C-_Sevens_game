#include "MyGameMapper.hpp"
#include "Generic_card_parser.hpp"
#include <algorithm>
#include <random>
#include <map>

namespace sevens {

std::vector<std::pair<uint64_t, uint64_t>>
MyGameMapper::compute_game_progress(uint64_t numPlayers)
{
    std::cout << "[MyGameMapper] Starting simulation with " << numPlayers << " players.\n";

    // 1. Générer et mélanger les cartes
    std::vector<Card> deck;
    for (uint64_t suit = 0; suit < 4; ++suit) {
        for (uint64_t rank = 1; rank <= 13; ++rank) {
            deck.push_back({suit, rank});
        }
    }
    std::shuffle(deck.begin(), deck.end(), rng_);

    // 2. Répartir les cartes aux joueurs
    playerHands_.clear();
    for (uint64_t i = 0; i < deck.size(); ++i) {
        uint64_t player = i % numPlayers;
        playerHands_[player].push_back(deck[i]);
    }

    // 3. Initialiser les stratégies
    for (uint64_t pid = 0; pid < numPlayers; ++pid) {
        strategies_[pid]->initialize(pid);
    }

    // 4. Initialiser le plateau avec les 7s
    tableLayout_.clear();
    for (uint64_t suit = 0; suit < 4; ++suit) {
        tableLayout_[suit][7] = true;
    }

    // 5. Boucle principale
    std::map<uint64_t, bool> finished;
    bool someoneFinished = false;
    uint64_t turn = 0;

    while (!someoneFinished) {
        uint64_t currentPlayer = turn % numPlayers;
        auto& strategy = strategies_[currentPlayer];
        auto& hand = playerHands_[currentPlayer];

        int index = strategy->selectCardToPlay(hand, tableLayout_);

        if (index >= 0 && static_cast<size_t>(index) < hand.size()) {
            Card card = hand[index];

            // Mettre la carte sur la table
            tableLayout_[card.suit][card.rank] = true;

            // Informer les autres
            for (uint64_t pid = 0; pid < numPlayers; ++pid) {
                if (pid != currentPlayer) {
                    strategies_[pid]->observeMove(currentPlayer, card);
                }
            }

            // Retirer la carte de la main
            hand.erase(hand.begin() + index);
        } else {
            for (uint64_t pid = 0; pid < numPlayers; ++pid) {
                if (pid != currentPlayer) {
                    strategies_[pid]->observePass(currentPlayer);
                }
            }
        }

        // Vérifier si quelqu’un a gagné
        if (hand.empty()) {
            finished[currentPlayer] = true;
            someoneFinished = true;
        }

        ++turn;
    }

    // 6. Retourner le nombre de cartes restantes pour chaque joueur
    std::vector<std::pair<uint64_t, uint64_t>> results;
    for (uint64_t pid = 0; pid < numPlayers; ++pid) {
        results.emplace_back(pid, playerHands_[pid].size());
    }

    return results;
}

void MyGameMapper::registerStrategy(uint64_t playerID, std::shared_ptr<PlayerStrategy> strategy) {
    strategies_[playerID] = strategy;
}

bool MyGameMapper::hasRegisteredStrategies() const {
    return !strategies_.empty();
}

void MyGameMapper::runCompetition(const std::vector<std::shared_ptr<PlayerStrategy>>& strategies) {
    for (uint64_t i = 0; i < strategies.size(); ++i) {
        registerStrategy(i, strategies[i]);
    }

    auto results = compute_game_progress(static_cast<uint64_t>(strategies.size()));

    std::cout << "\nRésultats finaux :\n";
    for (const auto& [pid, remaining] : results) {
        std::cout << "Player " << pid << " → " << remaining << " carte(s)\n";
    }
}

MyGameMapper::MyGameMapper() {
    rng_.seed(std::random_device{}());
}



// à remplir plus tard 
void MyGameMapper::read_cards(const std::string&) {}
void MyGameMapper::read_game(const std::string&) {}

std::vector<std::pair<uint64_t, uint64_t>> MyGameMapper::compute_and_display_game(uint64_t) {
    return {};
}

std::vector<std::pair<std::string, uint64_t>> MyGameMapper::compute_game_progress(const std::vector<std::string>&) {
    return {};
}

std::vector<std::pair<std::string, uint64_t>> MyGameMapper::compute_and_display_game(const std::vector<std::string>&) {
    return {};
}


} // namespace sevens

