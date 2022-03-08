#include "MinimumHopTree.h"
using namespace std;
long MinimumHopTree::getLastRegistrationTime(RouterID root, RouterID son) {
    if (lastOperationTime.find(root) == lastOperationTime.end()) {
        lastOperationTime[root] = unordered_map<RouterID, long>();
    }
    if (lastOperationTime[root].find(son) == lastOperationTime[root].end()) {
        lastOperationTime[root][son] = 0;
    }
    return lastOperationTime[root][son];
}

void MinimumHopTree::setLastRegistrationTime(RouterID root, RouterID son, long timestamp) {
    if (lastOperationTime.find(root) == lastOperationTime.end()) {
        lastOperationTime[root] = unordered_map<RouterID, long>();
    }
    lastOperationTime[root][son] = timestamp;
}

void MinimumHopTree::addToRegisteredSon(RouterID root, RouterID son) {
    if (registeredSons.find(root) == registeredSons.end()) {
        registeredSons[root] = vector<RouterID>();
    }

    for (int i = 0; i < registeredSons[root].size(); i++) {
        if (registeredSons[root][i] == son) {
            return;
        }
    }
    registeredSons[root].push_back(son);
}

void MinimumHopTree::deleteFromRegisteredSon(RouterID root, RouterID son) {
    if (registeredSons.find(root) == registeredSons.end()) {
        return;
    }
    for (auto itr = registeredSons[root].begin(); itr != registeredSons[root].end(); itr++) {
        if (*itr == son) {
            registeredSons[root].erase(itr);
            return;
        }
    }
}

pair<bool, RouterID> MinimumHopTree::getRegisteredParent(RouterID root) {
    if (registeredParents.find(root) == registeredParents.end()) {
        return {false, 0};
    }
    return {true, registeredParents[root]};
}

void MinimumHopTree::removeRegisteredParent(RouterID root) {
    auto itr = registeredParents.find(root);
    if (registeredParents.find(root) != registeredParents.end()) {
        registeredParents.erase(itr);
    }
}

void MinimumHopTree::setRegisteredParent(RouterID root, RouterID parent) { registeredParents[root] = parent; }

vector<RouterID> MinimumHopTree::getRegisteredSons(RouterID root) {
    if (registeredSons.find(root) != registeredSons.end()) {
        return registeredSons[root];
    }
    return vector<RouterID>();
}