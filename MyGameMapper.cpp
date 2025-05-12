#include "MyGameMapper.hpp"
#include "Generic_card_parser.hpp"
#include <algorithm>
#include <random>
#include <map>

namespace sevens {

void printCard(const sevens::Card& card) {
    static const char* suits[] = {"♣", "♦", "♥", "♠"};
    static const char* ranks[] = {"", "A", "2", "3", "4", "5", "6", "7",
                                  "8", "9", "10", "J", "Q", "K"};
    std::cout << ranks[card.rank] << suits[card.suit];
}

bool isPlayableCard(
    const sevens::Card& card,
    const std::unordered_map<uint64_t, std::unordered_map<uint64_t, bool>>& table)
{
    int suit = card.suit;
    int rank = card.rank;

    // Si aucune carte n’a encore été jouée dans cette couleur
    auto itSuit = table.find(suit);
    if (itSuit == table.end() || itSuit->second.empty()) {
        return rank == 7;  // seul le 7 peut démarrer une ligne
    }

    const auto& played = itSuit->second;
    return played.count(rank - 1) || played.count(rank + 1);
}


std::vector<std::pair<uint64_t, uint64_t>>
MyGameMapper::compute_game_progress(uint64_t numPlayers)
{
    std::cout << "[MyGameMapper] Starting simulation with " << numPlayers << " players.\n";

    // 1. Générer et mélanger les cartes
    std::vector<Card> deck;
    for (uint64_t suit = 0; suit < 4; ++suit) {
        for (uint64_t rank = 1; rank <= 13; ++rank) {
            deck.push_back({static_cast<int>(suit), static_cast<int>(rank)});
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
    
    //for (uint64_t suit = 0; suit < 4; ++suit) {
    //    tableLayout_[suit][7] = true;}
    

    std::map<uint64_t, bool> finished;
    bool someoneFinished = false;
    uint64_t turn = 0;

    std::cout << "\n--- Début du jeu ---\n";

    int passesInARow = 0;  // compteur de passes consécutives
    const int maxPassesBeforeBreak = static_cast<int>(numPlayers) * 5;

    while (!someoneFinished) {
        uint64_t currentPlayer = turn % numPlayers;
        auto& strategy = strategies_[currentPlayer];
        auto& hand = playerHands_[currentPlayer];

        int index = -1;
        for (size_t i = 0; i < hand.size(); ++i) {
            int proposed = strategy->selectCardToPlay(hand, tableLayout_);
            if (proposed >= 0 && static_cast<size_t>(proposed) < hand.size()) {
                if (isPlayableCard(hand[proposed], tableLayout_)) {
                    index = proposed;
                }
            }
            break; // n'appelle pas la stratégie en boucle, juste une fois
        }

        std::cout << "Tour " << turn + 1 << " - Joueur " << currentPlayer << " : ";

        if (index >= 0 && static_cast<size_t>(index) < hand.size()) {
            Card card = hand[index];
            printCard(card);
            std::cout << " joué\n";

            tableLayout_[card.suit][card.rank] = true;

            for (uint64_t pid = 0; pid < numPlayers; ++pid) {
                if (pid != currentPlayer) {
                    strategies_[pid]->observeMove(currentPlayer, card);
                }
            }

            hand.erase(hand.begin() + index);
            passesInARow = 0;  // réinitialiser le compteur si une carte est jouée
        } else {
            std::cout << "passe\n";

            for (uint64_t pid = 0; pid < numPlayers; ++pid) {
                if (pid != currentPlayer) {
                    strategies_[pid]->observePass(currentPlayer);
                }
            }

            passesInARow++;
        }

        // Affichage de la main restante
        std::cout << "Main de Joueur " << currentPlayer << " : ";
        for (const auto& c : hand) {
            printCard(c);
            std::cout << " ";
        }
        std::cout << "\n";

        // Vérifier victoire
        if (hand.empty()) {
            someoneFinished = true;
        }

        // Sécurité : jeu bloqué
        if (passesInARow >= maxPassesBeforeBreak) {
            std::cout << "💥 Jeu bloqué : tous les joueurs passent en boucle. Fin forcée.\n";
            break;
        }

        ++turn;
    }


    std::cout << "\n--- Résultats finaux détaillés ---\n";
    std::vector<std::pair<uint64_t, uint64_t>> results;
    for (uint64_t pid = 0; pid < numPlayers; ++pid) {
        int total = 0;
        std::cout << "Joueur " << pid << " : ";
        for (const auto& c : playerHands_[pid]) {
            printCard(c);
            std::cout << " ";
            total += c.rank;
        }
        std::cout << "→ Total points : " << total << " (" << playerHands_[pid].size() << " carte(s))\n";
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

