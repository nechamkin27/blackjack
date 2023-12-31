#include "blackjack.hpp"
#include <ctime>
#include <cstdlib>

#define GREEN_TEXT "\033[32m"
#define RED_TEXT "\033[31m"
#define RESET_TEXT "\033[0m"

// TODO: Seperate this version with another version that is intended for simulation.
//      The simulated version should not have text output for each turn, and should instead
//      print only the player value and hand number. Strategies for insurance, splitting hands,
//      and betting should be configurable.

// TODO: Fix deck to work like actual decks. Make the number of decks configurable.

// TODO: Add splitting hands.

// TODO: The program is crashing with a seg fault after a few turns. Determine the cause of this
//      (presumably from the deck) and fix.

Blackjack::Blackjack()
{
    player_score = 0;
    dealer_score = 0;
    player_cash = 250;
    player_cash_base = 250;
    current_bet = 0;
    payout = 0;
    initializeDeck();
    shuffleDeck();
}

void Blackjack::initializeDeck()
{
    const std::string suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    const std::string values[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};

    for (const std::string& suit : suits)
    {
        for (const std::string& value : values)
        {
            deck.push_back(value + " of " + suit);
        }
    }
}

void Blackjack::shuffleDeck()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = deck.size() - 1; i > 0; --i)
    {
        int j = std::rand() % (i + 1);
        std::swap(deck[i], deck[j]);
    }
}

void Blackjack::placeBet()
{
    int bet;
    std::cout << "How much would you like to bet?\n$";
    std::cin >> bet;
    std::cout << std::endl;

    if(bet <= 0 || bet > player_cash)
    {
        std::cout << "Invalid bet. Use value greater than 0 and less than current cash." << std::endl;
        placeBet();
    }
    else
    {
        current_bet = bet;
    }
}

void Blackjack::dealInitialCards()
{
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

void Blackjack::printHands(bool showDealerCard)
{
    std::cout << "Your hand: ";
    for (const std::string& card : player_hand)
    {
        if(card == player_hand.back())
            std::cout << card << "\n";
        else
            std::cout << card << ", ";
    }
    std::cout << "Your score: " << player_score << std::endl;

    std::cout << "Dealer's hand: ";
    for (size_t i = 0; i < dealer_hand.size(); i++)
    {
        if (i == 0 && !showDealerCard)
        {
            std::cout << "Face Down card, ";
        }
        else if(dealer_hand[i]!=dealer_hand.back())
        {
            std::cout << dealer_hand[i] << ", ";
        }
        else
        {
            std::cout << dealer_hand[i] << "\n";
        }
    }

    // Display the value of the second card in the dealer's hand when it's hidden.
    if (!showDealerCard && dealer_hand.size() >= 2)
    {
        std::cout << "Score: " << calculateScore(std::vector<std::string>{dealer_hand[1]});
    }
    else if (showDealerCard)
    {
        std::cout << "Score: " << dealer_score;
    }

    std::cout << std::endl;
}

bool Blackjack::playerTurn()
{
    printHands(false);

    char insurance;
    std::string face_up_card = dealer_hand[1].substr(0, dealer_hand[1].find(" of "));
    std::string face_down_card = dealer_hand[0].substr(0, dealer_hand[0].find(" of "));

    if(checkForBlackjack(face_up_card))
        return true;

    // If card showing is an Ace, ask if player would like insurance.
    if(face_up_card == "Ace")
    {
        std::cout << "Would you like insurance?" << std::endl;
        std::cin >> insurance;

       if(face_down_card == "King"
       || face_down_card == "Queen"
       || face_down_card == "Jack"
       || face_down_card == "10")
       {
            std::cout << "That's Blackjack, sorry!" << std::endl;

            if(insurance == 'Y' || insurance == 'y')
            {
                // Lose init bet, but win insurance 2:1. Net stay same.
                return true;
            }
            else if(insurance == 'N' || insurance == 'n')
            {
                // Lose init bet.
                player_cash -= current_bet;
                return true;
            }
        }
        else
        {
           if(insurance == 'Y' || insurance == 'y')
            {
                // Lose insurance, but hand continues.
                player_cash -= (current_bet*.5);
            }
            else if(insurance == 'N' || insurance == 'n')
            {
                // Player gambled and it paid off. No blackjack.
            }
        }
    }

    char choice;

    while (true)
    {
        bool firstTurn = (player_hand.size() == 2);

        if(firstTurn)
            std::cout << "Do you want to 'Hit' (H), 'Stand' (S), or 'Double Down' (D)?";
            // Check for insurance
            // Check for dealer blackjack
        else
        {
            printHands(false);
            std::cout << "Do you want to 'Hit' (H), or 'Stand' (S)?";
        }
        std::cin >> choice;
        std::cout << std::endl;
        if (choice == 'H' || choice == 'h')
        {
            if(checkForBust())
                return false;
        }
        else if (choice == 'S' || choice == 's')
        {
            return false;
        }
        else if (firstTurn && (choice == 'D' || choice == 'd'))
        {
            current_bet*=2;
            checkForBust();
            return false;
        }
        else
        {
            std::cout << "Invalid choice. Please enter 'H' or 'S'." << std::endl;
        }
    }
    return false;
}

bool Blackjack::checkForBlackjack(std::string& face_up_card)
{
    // If player has blackjack, payout 3 to 2!
    if(calculateScore(player_hand) == 21)
    {
        std::cout << "Blackjack baby!" << std::endl;
        player_cash += (current_bet * 1.5);
        return true;
    }

    // If dealer is showing a card value of 10, check if they have blackjack.
    if(face_up_card == "King"
    || face_up_card == "Queen"
    || face_up_card == "Jack"
    || face_up_card == "10")
    {
        if(calculateScore(dealer_hand) == 21)
        {
            std::cout << "Dealer has blackjack, sorry!";
            player_cash -= current_bet;
            return true;
        }
    }

    return false;
}

bool Blackjack::checkForBust()
{
    player_hand.push_back(deck.back());
    deck.pop_back();
    player_score = calculateScore(player_hand);
    if (isBust(player_hand))
    {
        printHands(true);
        std::cout << "Bust! You lose." << std::endl;
        player_cash -= current_bet;
        return true;
    }

    return false;
}

/* void Blackjack::checkInsurance()
{

} */

void Blackjack::dealerTurn()
{
    while (dealer_score < 17)
    {
        dealer_hand.push_back(deck.back());
        deck.pop_back();
        dealer_score = calculateScore(dealer_hand);
    }

    printHands(true);

    if (isBust(dealer_hand) || (dealer_score < player_score && player_score <= 21))
    {
        std::cout << "You won " << current_bet << "!\n";
        player_cash += current_bet;
    }
    else if (dealer_score > player_score)
    {
        std::cout << "Dealer wins." << std::endl;
        player_cash -= current_bet;
    }
    else
    {
        std::cout << "It's a push (tie)!" << std::endl;
    }
}

bool Blackjack::isBust(const std::vector<std::string>& hand)
{
    return calculateScore(hand) > 21;
}

int Blackjack::calculateScore(const std::vector<std::string>& hand)
{
    int score = 0;
    int numAces = 0;

    for (const std::string& card : hand)
    {
        std::string value = card.substr(0, card.find(" of "));

        if (value == "Ace")
        {
            score += 11;
            numAces++;
        }
        else if (value == "King" || value == "Queen" || value == "Jack")
        {
            score += 10;
        }
        else
        {
            score += std::stoi(value);
        }
    }

    while (score > 21 && numAces > 0)
    {
        score -= 10;
        numAces--;
    }

    return score;
}

// This is sloppy. Prints payout if endgame and current cash if not.
void Blackjack::printPayout(bool endGame)
{
    if(endGame)
    {
        std::cout << "Thanks for playing Blackjack!" << std::endl;
        if(payout < 0)
        {
            std::cout << "Your net payout is $" << RED_TEXT << payout << RESET_TEXT << std::endl;
            std::cout << "Ouch! Better luck next time!";
        }
        else if(payout > 0)
        {
            std::cout << "Your net payout is $" << GREEN_TEXT << payout << RESET_TEXT << std::endl;
            std::cout << "Wahoo! We're both glad you came!";
        }
    }

    else
    {
        if(payout < 0)
            std::cout << player_cash << "(" << RED_TEXT  << payout  << RESET_TEXT << ")" << std::endl;
        else if(payout > 0)
            std::cout << player_cash << "(+" << GREEN_TEXT  << payout  << RESET_TEXT << ")" << std::endl;
        else
            std::cout << player_cash << std::endl;
    }
}

void Blackjack::play()
{
    std::cout << "\nWelcome to Blackjack!\n" << std::endl;
    char playAgain;

    do
    {
        current_bet = 0;

        std::cout << "Current available cash: $";
        printPayout(false);
        placeBet();

        dealInitialCards();
        bool blackjack = playerTurn();

        if (!isBust(player_hand) && !blackjack)
        {
            dealerTurn();
        }

        std::cout << "Do you want to play again? (Y/N): ";
        std::cin >> playAgain;
        std::cout << std::endl;
        payout = player_cash - player_cash_base;
    }
    while (playAgain == 'Y' || playAgain == 'y');

    printPayout(true);
}

int main()
{
    Blackjack blackjackGame;
    blackjackGame.play();
    return 0;
}
