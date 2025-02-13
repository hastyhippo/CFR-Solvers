#include<bits/stdc++.h>
#include<random>
using namespace std;
typedef long long ll;

const char nl = '\n';

#define BET 1
#define CHECK 0
#define NUM_ACTIONS 2

const double threshold = 1e-4; 
struct node {
    vector<double> strategy;
    vector<double> strategySum;
    vector<double> regretSum;
};

vector<char> actions = {'c', 'b'};
vector<int> cards = {1,0,2};
map<string, node> position_to_node;

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

struct node *getStrategy(string history) {
    if (position_to_node.count(history) == 0) {
        position_to_node[history] = {
            vector<double>(NUM_ACTIONS, 1/(double)NUM_ACTIONS),
            vector<double>(NUM_ACTIONS, 0),
            vector<double>(NUM_ACTIONS, 0),
        };

    }
    return &position_to_node[history];
}

void normaliseStrategy(vector<double> &strategy, vector<double> regretSum) {
    double normalisedSum = 0;
    for (auto a : regretSum) {
        normalisedSum += max(0.0,a);
    }

    // If there is no regret sum, assume a uniform strategy
    if (normalisedSum == 0) {
        strategy = vector<double>(NUM_ACTIONS, (double)1/NUM_ACTIONS);
        return;
    }

    // Otherwise create a strategy weighted by regret
    for (int i = 0; i < NUM_ACTIONS; i++) {
        strategy[i] = max(0.0, regretSum[i]/normalisedSum);
    }
}

vector<double> getFinalStrategy(vector<double> &strategySum) {
    vector<double> finalStrategy(NUM_ACTIONS);
    double sum = 0;
    for (auto a : strategySum) {
        sum += a;
    }
    for (int i = 0; i < NUM_ACTIONS; i++) {
        finalStrategy[i] = (strategySum[i] / sum > threshold ? strategySum[i] / sum : 0);
    }
    return finalStrategy;
}

double getUtility(string history, int p1, int p2) {
    if (history == "cc") {
        return p1 > p2 ? 1 : -1;
    }
    if (history[history.length() - 1] == 'c') {
        return 1;
    }
    return p1 > p2 ? 2 : -2;
}


double CFR(string history, double p1, double p2, bool player) {
    
    // Terminating condition
    if (history.length() >= 2 && history != "cb") {
        return getUtility(history, player ? cards[0] : cards[1], player ? cards[1] : cards[0]);
    }
    struct node *currentNode = getStrategy(to_string(player ? cards[0] : cards[1]) + history);

    vector<double> strategy = currentNode->strategy;

    // Otherwise loop through and update CFR value for this history
    vector<double> action_val(NUM_ACTIONS, 0);
    double node_utility = 0;
    for (int i = 0; i < NUM_ACTIONS; i++) {
        // Recurse through the game tree to calculate all CFR values
        if (player) {
            action_val[i] = -CFR(history + actions[i], p1 * strategy[i], p2, !player);
        } else {
            action_val[i] = -CFR(history + actions[i], p1, p2 * strategy[i], !player);
        }
        node_utility += action_val[i] * strategy[i];
    }

    // Update the regret sum accordingly and add to strategy sum
    for (int i = 0; i < NUM_ACTIONS; i++) {
        currentNode->regretSum[i] += (player ? p2 : p1) * (action_val[i] - node_utility);
        currentNode->strategySum[i] += (player ? p1 : p2) *  strategy[i];
    }

    // Update the strategy based on current regret sum
    normaliseStrategy(currentNode->strategy, currentNode->regretSum);
    return node_utility;
}

void train(int iterations, double &avgSum) {
    random_device rd;
    mt19937 g(rd());

    while(iterations--) {
        shuffle(cards.begin(), cards.end(), g);
        avgSum += CFR("", 1, 1, true);
    }
}

int main() {
    double avgSum = 0;
    int iterations = 0;

    while(true) {
        int n;
        cout << "How many times to train: ";
        cin >> n;
        iterations += n;

        train(n, avgSum);
        for (auto a : position_to_node) {
            cout << a.first << " | ";
            cout << setprecision(5);
            print_v(getFinalStrategy(a.second.strategySum));
        }
        cout << avgSum / iterations << '\n';
    }
}