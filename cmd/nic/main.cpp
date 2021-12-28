#include<iostream>
#include"physicalLayer/nic/NicManager.h"
using namespace std;

int main(int argc, char* argv[]) {
    
    NicManager::getNicManager();
    auto tmp = NicManager::getNicManager()->getAllNicsInMap(false);
    for (auto i: tmp) {
        cout << i.second->toString() << endl;
    }
}