#include<bits/stdc++.h>
using namespace std;
typedef long long ll;

const char nl = '\n';

#define BET 1
#define CHECK 0
#define NUM_ACTIONS 2

struct node {
    vector<double> strategy;
    vector<double> strategySum;
    vector<double> regretSum;
};

vector<char> actions = {'b', 'c'};
vector<int> cards = {1,0,2};
map<string, node> position_to_node;
/*
    {"", {0,0,0}}, {"c", {0,0,0} }, {"b", {0,0,0}}, {"cc", {0,0,0}}, {"cb", {0,0,0}},
     {"bc", {0,0,0}}, {"bb", {0,0,0}}, {"cbb", {0,0,0}}, {"cbc", {0,0,0}}
*/

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
void printStrategy() {
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

double eval(string history, int p1, int p2) {
    if (history.substr(history.length() - 2)== "cc") {
        return p1 > p2 ? 1 : -1;
    }
    if (history[history.length() - 1] == 'c') {
        return 1;
    }
    return p1 > p2 ? 2 : -2;
}


double CFR(string history, double p1, double p2, bool player) {
    struct node *currentNode = getStrategy(history);
    
    //terminating condition
    if (history.length() >= 3 && history.substr(1, history.length()) != "cb") {
        return eval(history, player ? cards[0] : cards[1], player ? cards[1] : cards[0]);
    }

    vector<double> strategy = currentNode->strategy;

    //otherwise loop through and update CFR value for this history
    vector<double> action_val(NUM_ACTIONS, 0);
    double node_val = 0;
    for (int i = 0; i < NUM_ACTIONS; i++) {
        // recurse through the game tree to calculate all CFR values
        if (player) {
            action_val[i] = -CFR(history + actions[i], p1 * strategy[i], p2, !player);
        } else {
            action_val[i] = -CFR(history + actions[i], p1, p2 * strategy[i], !player);
        }
        node_val += action_val[i] * strategy[i];
    }

    // update the regret sum accordingly and add to strategy sum
    for (int i = 0; i < NUM_ACTIONS; i++) {
        currentNode->regretSum[i] += (player ? p2 : p1) * (action_val[i] - node_val);
        currentNode->strategySum[i] += (player ? p1 : p2) *  strategy[i];
    }
    print_v(currentNode->regretSum);
    print_v(currentNode->strategy);

    // Update the strategy based on current regret sum
    normaliseStrategy(currentNode->strategy, currentNode->regretSum);
    cout << history << " | " << node_val << "\n";
    return node_val;
}

void train(int iterations) {
    while(iterations--) {
        CFR("0", 1, 1, true);
    }
}

int main() {
    srand(time(0)*1000);

    /*
        regretSum
        strategySum
        strategy
    */
   for (int i = 0; i < 105; i++) {
        cout << CFR("0", 1, 1, true) << "\n\n";

   }
    
}