#include<bits/stdc++.h>
#include<random>
using namespace std;
typedef long long ll;

const char nl = '\n';

#define STAND 0
#define HIT 1
// #define DOUBLEDOWN 2
// #define SPLIT 3
#define NUM_ACTIONS 2

struct node {
    int strategy;
    double ev;
};

vector<string> move_name = {"STAND", "HIT", "DOUBLE", "SPLIT"};
const double threshold = 1e-3;
const int numDecks = 2;

const int16_t playerCountMask = 0b000000000111111;
const int16_t dealerCountMask = 0b000001111000000;
const int16_t canDoubleMask =   0b000010000000000;
const int16_t canSplitMask =    0b000100000000000;
const int16_t doubledMask =     0b001000000000000;
const int16_t hasAceMask =      0b010000000000000;
const int16_t standMask =       0b100000000000000;

int it = 0;

map<int16_t, node> position_to_node;

vector<int> cards;
/*
    5 4 3 2 1111 00000

    0 - playerCount
    1 - dealerCount
    2 - canDouble
    3 - canSplit
    4 - doubled
    5 - hasAce
    6 - isstand
*/

#pragma region 
bool canDouble(uint16_t history) {
    return (canDoubleMask & history) > 0;
}

bool canSplit(uint16_t history) {
    return (canSplitMask & history) > 0;
}

bool isDouble(uint16_t history) {
    return (doubledMask & history) > 0;
}

bool isStand(uint16_t history) {
    return (history & standMask) > 0;
}

bool hasAce(uint16_t history) {
    return (history & hasAceMask) > 0;
}

int getPlayerCount(uint16_t history) {
    return (playerCountMask & history);
}

int getDealerCount(uint16_t history) {
    return (dealerCountMask & history) >> 6;
}

void setPlayerCount(uint16_t &history, int count) {
    history &= ~playerCountMask;
    history |= count;
}

void setDealerCount(uint16_t &history, int count) {
    history &= ~dealerCountMask;
    history |= (count) << 6;
}

void setCanDouble(uint16_t &history, bool canDouble) {
    if (canDouble) {
        history |= canDoubleMask;
    } else {
        history &= ~canDoubleMask;
    }
}

void setHasAce(uint16_t &history, bool hasAce) {
    if (hasAce) {
        history |= hasAceMask;
    } else {
        history &= ~hasAceMask;
    }
}

void setDoubled(uint16_t &history) {
    history |= doubledMask;
}

void setStand(uint16_t &history) {
    history |= standMask;
}

void setCanSplit(uint16_t &history, bool canSplit) {
    if (canSplit) {
        history |= canSplitMask;
    } else {
        history &= ~canSplitMask;
    }
}

uint16_t CreateHistory(int playerCount, int dealerCount, bool canSplit, bool canDouble, bool hasAce) {
    uint16_t newHistory = 0;
    setPlayerCount(newHistory, playerCount);
    setDealerCount(newHistory, dealerCount);
    setCanSplit(newHistory, canSplit);
    setCanDouble(newHistory, canDouble);
    setHasAce(newHistory, hasAce);
    return newHistory;
}
#pragma endregion

void shuffle_deck() {
    random_device rd;
    mt19937 g(rd());

    shuffle(cards.begin(), cards.end(), g);
}

void printHistory(uint16_t h) {
    cout << "History: " << h << "\n";
    cout << "\t PlayerCount: " << getPlayerCount(h) << nl;
    cout << "\t DealerCount: " << getDealerCount(h) << nl;
    cout << (canDouble(h) ? "\t Can Double" : "\t Can't Double") << nl; 
    cout << (canSplit(h) ? "\t Can Split" : "\t Can't Split") << nl; 
    cout << (hasAce(h) ? "\t Has Ace" : "\t Doesn't have ace") << nl;
    cout << (isDouble(h) ? "\t Doubled" : "\t Not Doubled") << nl;
    cout << nl;
}

int getCard() {
    int card = cards[it++];
    if (it >= cards.size()) {
        it = 0;
        shuffle_deck();
    }
    return card;
}

// uint16_t makeMove(uint16_t startingHistory, int move) {
//     uint16_t newHistory = startingHistory;
//     int playerCount = getPlayerCount(startingHistory);
//     bool playerHasAce = hasAce(startingHistory);

//     // Disable doubling and splitting after the first move
//     setCanDouble(newHistory, false);
//     setCanSplit(newHistory, false);

//     if (move == STAND) {
//         setStand(newHistory);
//     } else if (move == HIT || move == DOUBLEDOWN) {
//         int card = getCard();

//         // Update player count and handle Aces
//         if (card == 11) {
//             if (playerCount + card > 21) {
//                 playerCount += 1;
//             } else {
//                 playerCount += 11;
//                 playerHasAce = true;
//             }
//         } else {
//             playerCount += card;
//         }

//         // Adjust for Aces if the count exceeds 21
//         if (playerCount > 21 && playerHasAce) {
//             playerCount -= 10; // Treat Ace as 1 instead of 11
//             playerHasAce = false;
//         }

//         // Update the new history
//         setPlayerCount(newHistory, playerCount);
//         setHasAce(newHistory, playerHasAce);

//         // Handle doubling down
//         if (move == DOUBLEDOWN) {
//             setDoubled(newHistory);
//         }
//     } else if (move == SPLIT) {
//         // Handle splitting (not implemented in this example)
//         // This would require creating two new hands and updating the game state accordingly.
//     }

//     return newHistory;
// }

int resultAfterDealing(uint16_t history, bool print) {
    int dealerCount = getDealerCount(history);
    int playerCount = getPlayerCount(history);

    if (playerCount > 21 && hasAce(history)) {
        playerCount -= 10;
    }

    bool dealerHasAce = (dealerCount == 11);
    if (print) cout << "start count: " << dealerCount<< "\n";

    while (dealerCount < 17 || (dealerHasAce && dealerCount == 17)) {
        int card = getCard();
        if (print) cout << "card: " << card << "  ";

        if (card == 11) {
            if (card + dealerCount > 21) {
                dealerCount += 1;
            } else {
                dealerCount += 11;
                dealerHasAce = true;
            }
        } else {
            dealerCount += card;
        }

        if (dealerCount > 21 && dealerHasAce) {
            dealerCount -= 10;
            dealerHasAce = false;
        }
        if (dealerCount > 21) {
            if (print) cout << "dealer count: " << dealerCount<< "\n";
            return 1;
        }
    }
    if (print) cout << "dealer count: " << dealerCount<< "\n";

    if(print) cout << "Dealer: " << dealerCount << " | player: " << playerCount << "\n"; 
    if (dealerCount > playerCount) return -1;
    else if (dealerCount == playerCount) return 0; 
    else return 1;
}

double MCSearch(uint16_t history, bool print) {

}

void train(int iterations) {

    double sum = 0;
    uint16_t history = 0;

    for(int i = 0; i <iterations; i++) {
        int c1 = getCard(), c2 = getCard(), c3 = getCard();
        // cin >> c1 >> c2 >> c3;
        if (c1 + c2 == 21) {
            if (c3 + getCard() == 21) {
                sum += 0;
            } else {
                sum += 1.5;
            }
        } else {
            // int c1 = 10, c2 = 11, c3 = 10;
            if (c1 == c2) setCanSplit(history, true);
            setCanDouble(history, true);
            if (c1 == 11 || c2 == 11) setHasAce(history, true);
            setPlayerCount(history, c1 + c2);
            setDealerCount(history, c3);
            double d = MCSearch(history, false);
            // cout << "\n";
            cout << "Ev: "<< d << '\n';
            sum += d;
        }

    }
    cout << sum << " \n";
    cout << "Ev: "<< sum/iterations << '\n';
    // cout << move_name[i] << ": " << ((strategy[i] < threshold )? 0 : strategy[i] ) << " | ";
    cout << "\n";


}


int main() {
    double avgSum = 0;
    int iterations = 0;
    for (int i = 0; i < 4 * numDecks; i++) {
        for (int j = 2; j < 12; j++) {
            cards.push_back(j);
        }
        for (int j = 0; j < 3; j++) {
            cards.push_back(10);
        }
    }    
    
    while(true) {

        int n;
        // cout << "How many times to train: ";
        shuffle_deck();
        uint16_t start = 0;
        cin >> n;

        train(n);

        // while(n--) {

        //     makeMove(start, HIT);
        //     cout << getPlayerCount(start) << nl;

        // }
    }
}