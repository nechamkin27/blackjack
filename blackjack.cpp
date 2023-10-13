#include "blackjack.hpp"
#include <ctime>
#include <cstdlib>

Blackjack::Blackjack() {
    player_score = 0;
    dealer_score = 0;
    initializeDeck();
    shuffleDeck();
}

void Blackjack::initializeDeck() {
    const std::string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    const std::string values[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};
    
    for (const std::string& suit : suits) {
        for (const std::string& value : values) {
            deck.push_back(value + " of " + suit);
        }
    }
}

void Blackjack::shuffleDeck() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = deck.size() - 1; i > 0; --i) {
        int j = std::rand() % (i + 1);
        std::swap(deck[i], deck[j]);
    }
}

void Blackjack::dealInitialCards() {
    player_hand.clear();
    dealer_hand.clear();
    
    player_hand.push_back(deck.back());
    deck.pop_back();
    dealer_hand.push_back(deck.back());
    deck.pop_back();
    player_hand.push_back(deck.back());
    deck.pop_back();
    dealer_hand.push_back(deck.back());
    deck.pop_back();
    
    player_score = calculateScore(player_hand);
    dealer_score = calculateScore(dealer_hand);
}

void Blackjack::printHands(bool showDealerCard) {
    std::cout << "Your hand: ";
    for (const std::string& card : player_hand) {
        std::cout << card << " ";
    }
    std::cout << "Score: " << player_score << std::endl;
    
    std::cout << "Dealer's hand: ";
    for (size_t i = 0; i < dealer_hand.size(); i++) {
        if (i == 0 && !showDealerCard) {
            std::cout << "Face Down ";
        } else {
            std::cout << dealer_hand[i] << " ";
        }
    }
    std::cout << "Score: " << (showDealerCard ? dealer_score : 0) << std::endl;
}

void Blackjack::playerTurn() {
    char choice;
    while (true) {
        printHands(false);
        std::cout << "Do you want to 'Hit' (H) or 'Stand' (S)? ";
        std::cin >> choice;
        if (choice == 'H' || choice == 'h') {
            player_hand.push_back(deck.back());
            deck.pop_back();
            player_score = calculateScore(player_hand);
            if (isBust(player_hand)) {
                printHands(true);
                std::cout << "Bust! You lose." << std::endl;
                return;
            }
        } else if (choice == 'S' || choice == 's') {
            return;
        } else {
            std::cout << "Invalid choice. Please enter 'H' or 'S'." << std::endl;
        }
    }
}

void Blackjack::dealerTurn() {
    while (dealer_score < 17) {
        dealer_hand.push_back(deck.back());
        deck.pop_back();
        dealer_score = calculateScore(dealer_hand);
    }
    
    printHands(true);
    
    if (isBust(dealer_hand) || (dealer_score < player_score && player_score <= 21)) {
        std::cout << "You win!" << std::endl;
    } else if (dealer_score > player_score) {
        std::cout << "Dealer wins." << std::endl;
    } else {
        std::cout << "It's a push (tie)!" << std::endl;
    }
}

bool Blackjack::isBust(const std::vector<std::string>& hand) {
    return calculateScore(hand) > 21;
}

int Blackjack::calculateScore(const std::vector<std::string>& hand) {
    int score = 0;
    int numAces = 0;
    
    for (const std::string& card : hand) {
        std::string value = card.substr(0, card.find(" of "));
        
        if (value == "Ace") {
            score += 11;
            numAces++;
        } else if (value == "King" || value == "Queen" || value == "Jack") {
            score += 10;
        } else {
            score += std::stoi(value);
        }
    }
    
    while (score > 21 && numAces > 0) {
        score -= 10;
        numAces--;
    }
    
    return score;
}

void Blackjack::play() {
    std::cout << "Welcome to Blackjack!" << std::endl;
    char playAgain;
    
    do {
        dealInitialCards();
        printHands(false);
        playerTurn();
        
        if (!isBust(player_hand)) {
            dealerTurn();
        }
        
        std::cout << "Do you want to play again? (Y/N): ";
        std::cin >> playAgain;
    } while (playAgain == 'Y' || playAgain == 'y');
    
    std::cout << "Thanks for playing Blackjack!" << std::endl;
}

int main() {
    Blackjack blackjackGame;
    blackjackGame.play();
    return 0;
}