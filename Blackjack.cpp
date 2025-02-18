#include<bits/stdc++.h>
#include<random>
using namespace std;
typedef long long ll;

const char nl = '\n';

#define STAND 0
#define HIT 1
#define DOUBLE 2
#define NUM_ACTIONS 3

struct node {
    vector<double> ev;
};

vector<string> move_name = {"STAND", "HIT", "DOUBLE"};
const double threshold = 1e-3;
const int numDecks = 2;

const uint32_t playerCountMask = 0b000000000111111;
const uint32_t dealerCountMask = 0b000011111000000;
const uint32_t canDoubleMask =   0b000100000000000;
const uint32_t canSplitMask =    0b001000000000000;
const uint32_t doubledMask =     0b010000000000000;
const uint32_t hasAceMask =      0b100000000000000;
const uint32_t standMask =       0b1000000000000000;

const int MC_iterations = 10000;
int it = 0;

unordered_map<uint32_t, node> position_to_node;

vector<int> cards;

#pragma region 
bool canDouble(uint32_t history) {
    return (canDoubleMask & history) > 0;
}

bool canSplit(uint32_t history) {
    return (canSplitMask & history) > 0;
}

bool isDouble(uint32_t history) {
    return (doubledMask & history) > 0;
}

bool isStand(uint32_t history) {
    return (history & standMask) > 0;
}

bool hasAce(uint32_t history) {
    return (history & hasAceMask) > 0;
}

int getPlayerCount(uint32_t history) {
    return (playerCountMask & history);
}

int getDealerCount(uint32_t history) {
    return (dealerCountMask & history) >> 6;
}

void setPlayerCount(uint32_t &history, int count) {
    assert(count >= 0 && count <= 63);

    history &= ~playerCountMask;
    history |= (count & playerCountMask);
}

void setDealerCount(uint32_t &history, int count) {
    assert(count >= 0 && count <= 31);
    history &= ~dealerCountMask;
    history |= (((count) << 6) & dealerCountMask);
}

void setCanDouble(uint32_t &history, bool canDouble) {
    if (canDouble) {
        history |= canDoubleMask;
    } else {
        history &= ~canDoubleMask;
    }
}

void setHasAce(uint32_t &history, bool hasAce) {
    if (hasAce) {
        history |= hasAceMask;
    } else {
        history &= ~hasAceMask;
    }
}

void setDoubled(uint32_t &history) {
    history |= doubledMask;
}

void setStand(uint32_t &history) {
    history |= standMask;
}

void setCanSplit(uint32_t &history, bool canSplit) {
    if (canSplit) {
        history |= canSplitMask;
    } else {
        history &= ~canSplitMask;
    }
}

uint32_t CreateHistory(int playerCount, int dealerCount, bool canSplit, bool canDouble, bool hasAce) {
    uint32_t newHistory = 0;
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

void printHistory(uint32_t h) {
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

uint32_t makeMove(uint32_t startingHistory, int move) {
    uint32_t newHistory = startingHistory;
    int playerCount = getPlayerCount(startingHistory);
    bool playerHasAce = hasAce(startingHistory);

    setCanDouble(newHistory, false);
    setCanSplit(newHistory, false);

    if (move == STAND) {
        setStand(newHistory);
    } else if (move == HIT || move == DOUBLE) {
        int card = getCard();

        if (card == 11) {
            if (playerCount + card > 21) {
                playerCount += 1;
            } else {
                playerCount += 11;
                playerHasAce = true;
            }
        } else {
            playerCount += card;
        }

        if (playerCount > 21 && playerHasAce) {
            playerCount -= 10;
            playerHasAce = false;
        }

        setPlayerCount(newHistory, playerCount);
        setHasAce(newHistory, playerHasAce);
        setCanDouble(newHistory, false);
        if (move == DOUBLE) {
            setDoubled(newHistory);
            setStand(newHistory);
        }
    }

    return newHistory;
}

int resultAfterDealing(uint32_t history, bool print) {
    int dealerCount = getDealerCount(history);
    int playerCount = getPlayerCount(history);

    if (playerCount > 21 && hasAce(history)) {
        playerCount -= 10;
    }

    bool dealerHasAce = (dealerCount == 11);
    if (print) cout << "start count: " << dealerCount<< "\n";
    int it = 0;
    while (dealerCount < 17 || (dealerHasAce && dealerCount == 17)) {
        int card = getCard();

        if (it == 0) {
            if (dealerCount == 10) {
                while(card == 11) {
                    card = getCard();
                }
            } else if (dealerCount == 11) {
                while (card == 10) {
                    card = getCard();
                }
            }
        }

        if (print) cout << "card: " << card;

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
        if(print) cout << " | " << dealerCount << "\n";

        if (dealerCount > 21) {
            if (print) cout << "dealer bust with: " << dealerCount<< "\n";
            return 1;
        }
        it++;
    }
    if (print) cout << "dealer count: " << dealerCount<< "\n";

    if(print) cout << "Dealer: " << dealerCount << " | player: " << playerCount << "\n"; 
    if (dealerCount > playerCount) return -1;
    else if (dealerCount == playerCount) return 0; 
    else return 1;
}

bool terminalNode(uint32_t history) {
    if (getPlayerCount(history) > 21) return true;
    if (isStand(history) || isDouble(history)) return true;
    return false;
}

double nodeUtility(uint32_t history) {
    if (getPlayerCount(history) > 21) return(isDouble(history) ? 2: 1)* -1;
    if (isStand(history) || isDouble(history)) {
        double result = resultAfterDealing(history, false);
        return (isDouble(history) ? 2: 1)* result  ;
    }
    return 0;
}

vector<double> MCSearch(uint32_t history, bool print) {
    if (position_to_node.count(history) != 0) {
        return position_to_node[history].ev;
    }
    vector<double> action_ev(NUM_ACTIONS);
    for (int i = 0; i < NUM_ACTIONS; i++) {
        if(!canDouble(history) && i == DOUBLE) {
            action_ev[i] = -100000000;
            continue;
        }
        for (int j = 0; j < MC_iterations; j++) {
            uint32_t new_history = makeMove(history, i);
            // cout << move_name[i];
            if (terminalNode(new_history)) {
                // cout << nodeUtility(new_history) << '\n';
                action_ev[i] += nodeUtility(new_history);
            } else {
                if (position_to_node.count(new_history) == 0) {
                    vector<double> p = MCSearch(new_history, print);
                    position_to_node[new_history] = {p};
                } 
                action_ev[i] += *max_element(position_to_node[new_history].ev.begin(), position_to_node[new_history].ev.end());
            }
        }

        action_ev[i] /= MC_iterations;
    }

    printHistory(history);
    for (int i = 0; i<  NUM_ACTIONS; i++) {
        cout << move_name[i] << " : " << action_ev[i] << " | ";
    }
    cout << '\n';
    printHistory(history);
    position_to_node[history] = {action_ev};
    return action_ev;
}

int getBestMove(vector<double> ev, bool canDouble, bool canSplit) {
    int bestMove = 0;
    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (ev[i] > ev[bestMove]) {
            bestMove = i;
        }
    }
    return bestMove;
}

void printTable(){
    cout << fixed << setprecision(4);

    cout << "WITHOUT ACE: \n\n";
    cout << setw(5) << "P\\D"; // Header row
    for (int j = 2; j <= 11; j++) {
        cout << setw(8) << j;
    }
    cout << '\n' << string(88, '-') << '\n';

    for (int i = 4; i <= 20; i++) {
        cout << setw(5) << i << " |";
        for (int j = 2; j <= 11; j++) {
            uint32_t h = CreateHistory(i, j, false, false, false);
            if (position_to_node.count(h) != 0) {
                cout << setw(8) << *max_element(position_to_node[h].ev.begin(), position_to_node[h].ev.end());
            } else {
                cout << setw(8) << " ";
            }
        }
        cout << '\n';
    }
    cout << "\n\n";
    for (int i = 4; i <= 20; i++) {
        cout << setw(5) << i << " |";
        for (int j = 2; j <= 11; j++) {
            uint32_t h = CreateHistory(i, j, false, true, false);
            if (position_to_node.count(h) != 0) {
                cout << setw(8) << move_name[getBestMove(position_to_node[h].ev, false, false)];
            } else {
                cout << setw(8) << " ";
            }
        }
        cout << '\n';
    }

    cout << "\n\nWITH ACE: \n\n";
    cout << setw(5) << "P\\D";
    for (int j = 2; j <= 10; j++) {
        cout << setw(8) << j;
    }
    cout << '\n' << string(88, '-') << '\n';

    for (int i = 12; i <= 21; i++) {
        cout << setw(5) << i << " |";
        for (int j = 2; j <= 11; j++) {
            uint32_t h = CreateHistory(i, j, false, false, true);
            if (position_to_node.count(h) != 0) {
                cout << setw(8) << *max_element(position_to_node[h].ev.begin(), position_to_node[h].ev.end());
            } else {
                cout << setw(8) << " ";
            }
        }
        cout << '\n';
    }

    cout << "\n\n";
    for (int i = 12; i <= 21; i++) {
        cout << setw(5) << i << " |";
        for (int j = 2; j <= 11; j++) {
            uint32_t h = CreateHistory(i, j, false, false, true);
            if (position_to_node.count(h) != 0) {
                cout << setw(8) << move_name[getBestMove(position_to_node[h].ev, true, false)];
            } else {
                cout << setw(8) << " ";
            }
        }
        cout << '\n';
    }

}

void train(int iterations) {
    double sum = 0;

    for(int i = 0; i < iterations; i++) {
        uint32_t history = 0;

        shuffle_deck();
        int c1 = getCard(), c2 = getCard(), c3 = getCard(), c4 = getCard();
        if (c1 + c2 == 21) {
            if (c3 + getCard() == 21) {
                sum += 0;
            } else {
                sum += 1.5;
            }
        } else if (c3 + c4 == 21) {
            sum += -1;
        } else {
            if (c1 == 11 || c2 == 11) setHasAce(history, true);
            if (c1 == 11 && c2 == 11) c1 -= 10;
            // if (c3 == 11 && c4 == 11) c3 -= 10;
            setPlayerCount(history, c1 + c2);
            setDealerCount(history, c3);
            setCanDouble(history, true);
            vector<double> result = MCSearch(history, false);
            sum += *max_element(result.begin(), result.end());
        }
    }

    // Formatting the table
    printTable();
    cout << "EV: " << sum / iterations << '\n';
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
    shuffle_deck();

    shuffle_deck();
    uint32_t start = 0;
    int n = 100000;
    train(n);
}