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
    double player_cash;
    int player_cash_base;
    int current_bet;
    int payout;
    int numDecks;
    int numTurns;
    int currTurn;

    void initializeDeck(int numDecks);
    void shuffleDeck();
    void placeBet();
    void dealInitialCards();
    void printHands(bool showDealerCard);
    bool playerTurn();
    void dealerTurn();
    bool checkForBust();
    bool checkForBlackjack(std::string& face_up_card);
    bool isBust(const std::vector<std::string>& hand);
    int calculateScore(const std::vector<std::string>& hand);
    void printPayout(bool endGame);
    void reshuffleDeck();
    bool promptPlayAgain();
};

#endif // BLACKJACK_HPP