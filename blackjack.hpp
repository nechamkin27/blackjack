#ifndef BLACKJACK_HPP
#define BLACKJACK_HPP

#include <iostream>
#include <vector>
#include <string>

class Blackjack {
public:
    Blackjack();
    void play();
    
private:
    std::vector<std::string> deck;
    std::vector<std::string> player_hand;
    std::vector<std::string> dealer_hand;
    
    int player_score;
    int dealer_score;
    
    void initializeDeck();
    void shuffleDeck();
    void dealInitialCards();
    void printHands(bool showDealerCard);
    void playerTurn();
    void dealerTurn();
    bool isBust(const std::vector<std::string>& hand);
    int calculateScore(const std::vector<std::string>& hand);
};

#endif // BLACKJACK_HPP