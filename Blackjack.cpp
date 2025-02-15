#include<bits/stdc++.h>
#include<random>
using namespace std;
typedef long long ll;

const char nl = '\n';

#define STAND 0
#define HIT 1
#define DOUBLEDOWN 2
#define SPLIT 3
#define NUM_ACTIONS 4

struct node {
    vector<double> strategy;
    vector<double> strategySum;
    vector<double> regretSum;
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

uint16_t makeMove(uint16_t startingHistory, int move) {
    uint16_t newHistory = startingHistory;
    setCanDouble(newHistory, false);
    if (move == STAND) {
        setStand(newHistory);
    } else if (move == HIT) {
        int card = getCard();
        // cout << "CARD IS " << card << " \n\n";
        setPlayerCount(newHistory, getPlayerCount(newHistory) + card);
        setCanDouble(newHistory, false);
        setCanSplit(newHistory, false);
        if (card == 11) setHasAce(newHistory, true);
    } else if (move == DOUBLEDOWN) {
        int card = getCard();
        // cout << "CARD IS " << card << " \n\n";

        setPlayerCount(newHistory, getPlayerCount(newHistory) + card);
        setCanDouble(newHistory, false);
        setDoubled(newHistory);
        setCanSplit(newHistory, false);
        if (card == 11) setHasAce(newHistory, true);
    }
    return newHistory;
}

int resultAfterDealing(uint16_t history) {
    int dealerCount = getDealerCount(history);
    int playerCount = getPlayerCount(history);
    if (playerCount > 21 && hasAce(history)) playerCount -=10;
    bool hasAce = dealerCount == 11;
    // cout << "start: " << dealerCount << " \n";

    while (true) {
        int card = getCard();
        if (hasAce) {
            if (dealerCount > 31) return -1;
            if ((dealerCount >= 17 && dealerCount <= 21) || dealerCount >= 27) {
                // cout << '\n' <<  playerCount   << " (ace)| " << dealerCount << '\n';
                if (dealerCount > playerCount) return -1;
                if (dealerCount == playerCount) return 0;
                if (dealerCount < playerCount) return 1;
            } 
            // cout << card << " + " << dealerCount << " = " << dealerCount + card << "\n";

            dealerCount += card;
        } else {
            if (dealerCount >= 17) {
                // cout << '\n' <<  playerCount   << " | " << dealerCount << '\n';
                if (dealerCount > 21) return 1;
                if (dealerCount > playerCount) return -1;
                if (dealerCount == playerCount) return 0;
                if (dealerCount < playerCount) return 1;
            }
            if (card == 11) hasAce = true;
            // cout << card << " + " << dealerCount << " = " << dealerCount + card << "\n";
            dealerCount += card;
        }
 
    }
}

int getAction(vector<double> strategy, int size) {
    double r = (double)rand() / RAND_MAX;
    double cumulativeSum = 0;
    int i = 0;
    for (; i < NUM_ACTIONS; i++) {
        cumulativeSum += strategy[i];
        if (cumulativeSum >= r) {
            break;
        }
    }
    return i;
}

void print_v(vector<double> v) {
    for (auto a : v) cout << a << " ";
    cout << '\n';
}

struct node *getStrategy(uint16_t history) {
    if (position_to_node.count(history) == 0) {
        double n = 2 + canDouble(history) + canSplit(history);
        position_to_node[history] = {
            {1/n, 1/n, (canDouble(history) * (1/n)), (canSplit(history) * (1/n))},
            vector<double>(NUM_ACTIONS),
            vector<double>(NUM_ACTIONS),
        };
    }
    return &position_to_node[history];
}

void normaliseStrategy(vector<double> &strategy, vector<double> regretSum) {
    double sum = 0;
    for (auto a : regretSum) sum += max(a, 0.0);
    if (sum ==  0) {
        strategy = {1/2,1/2,0,0}; return;
    } 
    for (int i = 0; i < 4; i++) {
        strategy[i] = (max(regretSum[i], 0.0)) / sum;
    }
    return;
}

vector<double> getFinalStrategy(vector<double>strategySum) {
    double sum = 0;
    for (auto a : strategySum) sum += a;
    if (sum == 0) {
        return {1/2,1/2,0,0};
    }
    vector<double> finalStrategy(NUM_ACTIONS);
    for (int i = 0; i < NUM_ACTIONS; i++) {
        finalStrategy[i] = strategySum[i] / sum;
    }
    return finalStrategy;
}

double getUtility(uint16_t history) {
    return 0;
}

double CFR(uint16_t history, double p1) {
    // if terminal node
    // printHistory(history);
    if ((!hasAce(history) && getPlayerCount(history) > 21) || (hasAce(history) && getPlayerCount(history) > 31)) {
        // cout << "Returning: " << (isDouble(history) ? -2 : -1) << "\n\n";
        return isDouble(history) ? -2 : -1;
    }
    if (isStand(history) || isDouble(history)) {
        // cout << "Returning: " << (isDouble(history) ? 2 : 1) * (resultAfterDealing(history)) << "\n\n";
        return (isDouble(history) ? 2 : 1) * (resultAfterDealing(history));
    }

    struct node *currentNode = getStrategy(history);
    vector<double> strategy = currentNode->strategy;
    double node_util = 0;
    vector<double> action_util(NUM_ACTIONS,0);

    vector<bool> possible_moves = {1,1, canDouble(history), canSplit(history)};
    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (!possible_moves[i]) continue;
        if (i == SPLIT) {
            int count = getPlayerCount(history) / 2;
            for (int i = 0; i < 2; i++) {
                int new_card = getCard();
                uint16_t new_history = CreateHistory(
                    count + new_card, getDealerCount(history), false, true, new_card == 11 || count == 11
                );
                action_util[i] += CFR(new_history, p1 * strategy[i]);
                node_util += action_util[i] * strategy[i];
            }
        } else {
            // cout << "MOVE: " << move_name[i] << '\n';
            action_util[i] += CFR(makeMove(history, i), p1 * strategy[i]);
            node_util += action_util[i] * strategy[i];
        }
    }

    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (possible_moves[i]) {
            currentNode->regretSum[i] += (action_util[i] - node_util) * p1;
            currentNode->strategySum[i] += strategy[i] * p1;
        }
    }

    normaliseStrategy(currentNode->strategy, currentNode->regretSum);

    // cout << "Player: " << getPlayerCount(history) << " | Dealer: " << getDealerCount(history) << "\n"; 
    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (possible_moves[i]) {
            // cout << move_name[i] << " | " << action_util[i] << '\t';
        }
    }
    // cout << "\n";
    return 0;
}

void train(int iterations) {
    int playercount, dealercount, candouble, cansplit, hasace;
    cout << "Playercount: ";
    cin >> playercount;

    for (int i = 2; i <= 11; i++) {

        uint16_t new_hist = CreateHistory(playercount, i, false, true, false);
        printHistory(new_hist);
        
        for(int i = 0; i <iterations; i++) {
            // uint16_t history = 0;
            // int c1 = getCard(), c2 = getCard(), c3 = getCard();
            // // int c1 = 10, c2 = 11, c3 = 10;
            // if (c1 == c2) setCanSplit(history, true);
            // setCanDouble(history, true);
            // if (c1 == 11 || c2 == 11) setHasAce(history, true);
            // cout << c1 << " " << c2 << " " << c3 << nl;
            // setPlayerCount(history, c1 + c2);
            // setDealerCount(history, c3);
            // CFR(history, 1);
            CFR(new_hist, 1);
        }
        struct node n = position_to_node[new_hist];
        vector<double> strategy = getFinalStrategy(n.strategySum);
        cout << setprecision(3);
        for (int i = 0; i < NUM_ACTIONS; i++){
            if (strategy[i] > 0.5) {
                cout << move_name[i] << '\n';
                break;
            }
        }
        // cout << move_name[i] << ": " << ((strategy[i] < threshold )? 0 : strategy[i] ) << " | ";
        cout << "\n";
    }

        // for (auto a : n.strategySum) cout << a << " ";
        // cout << '\n';
        // for (auto a : n.regretSum) cout << a << " ";
        // cout << '\n';
    // for (auto a : position_to_node) {
    //     printHistory(a.first);
    //     vector<double> strategy = getFinalStrategy(a.second.strategySum);
    //     for (auto a  : strategy) cout << a << " ";
    //     cout << "\n";
    //     for (auto a : a.second.strategySum) cout << a << " ";
    //     cout << '\n';
    //     for (auto a : a.second.regretSum) cout << a << " ";
    //     cout << '\n';
    // }

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

        train(300000);
        // while(n--) {

        //     makeMove(start, HIT);
        //     cout << getPlayerCount(start) << nl;

        // }
    }
}